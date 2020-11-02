// $Id: file_sys.cpp,v 1.8 2020-10-22 14:37:26-07 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <iomanip>      // std::setw

using namespace std;

#include "debug.h"
#include "file_sys.h"

size_t inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

inode_state::inode_state() {
   root = make_shared<inode>(file_type::DIRECTORY_TYPE);
   cwd = root;
   root->contents->get_dirents().insert(
      pair<string, inode_ptr>(".",root));
   root->contents->get_dirents().insert(
      pair<string, inode_ptr>("..",root));
   root->contents->set_path("/");
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
      << ", prompt = \"" << prompt() << "\"");
}


const string& inode_state::prompt() const { return prompt_; }

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>(); 
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>(); 
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

size_t inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}


file_error::file_error (const string& what):
            runtime_error (what) {
}

const wordvec& base_file::readfile() const {
   throw file_error ("is a " + error_file_type());
}

void base_file::writefile (const wordvec&) {
   throw file_error ("is a " + error_file_type());
}

void base_file::remove (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkdir (const string&) {
   throw file_error ("is a " + error_file_type());
}

inode_ptr base_file::mkfile (const string&) {
   throw file_error ("is a " + error_file_type());
}

size_t plain_file::size() const { 
   size_t size {0};
   for (auto i : this->data) {
      size += i.length();
   }
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   wordvec::const_iterator it = words.begin();
   it += 2;
   this->data.clear();
   while (it != words.end())
   {
      this->data.push_back(*it);
      it++;
   }
   DEBUGF ('i', words);
}

size_t directory::size() const {
   size_t size = this->dirents.size();
   DEBUGF ('i', "size = " << size);
   return size;
}

void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
   if (this->dirents.find(filename) != this->dirents.end()) {
      try {
         auto new_dirents = 
            this->dirents[filename]->get_contents()->get_dirents();
         if (new_dirents.size() < 3) {
            this->dirents[filename]->get_contents() = nullptr;
            this->dirents.erase(filename);
            return;
         }
         else { cout << "Directory is not empty." << endl; return; }
      }
      catch(std::exception const& e) {
         this->dirents[filename]->get_contents() = nullptr;
         this->dirents.erase(filename);
         return;
      }
   }
   else {
      cout << "File does not exist." << endl;
   }
}

inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   inode_ptr dir = make_shared<inode>(file_type::DIRECTORY_TYPE);  
   string parent_path = this->get_path(); 
   string new_path = parent_path + dirname + "/";
   dir->get_contents()->set_path(new_path);
   this->dirents.insert(pair<string, inode_ptr>(dirname,dir));
   dir->get_contents()->get_dirents().insert(
      pair<string, inode_ptr>(".",dir));
   dir->get_contents()->get_dirents().insert(
      pair<string, inode_ptr>("..",this->get_dirents().at(".")));
   return dir;
}

inode_ptr directory::mkfile (const string& filename) {
   inode_ptr file = make_shared<inode>(file_type::PLAIN_TYPE);  
   this->dirents.insert(pair<string, inode_ptr>(filename,file));
   string parent_path = this->get_path(); 
   string new_path = parent_path + filename + "";
   file->get_contents()->set_path(new_path);
   DEBUGF ('i', filename);
   return file;
}

void directory::print_dirents() const {
   auto _path = this->path;
   if (_path.length() <  2) cout << "/: " << endl;
   else cout << path.substr(0, _path.size()-1) << ":" << endl; 
   map<string, inode_ptr>::const_iterator it = this->dirents.begin();
   while (it != this->dirents.end())
   {
      cout << setw(6) << it->second->get_inode_nr() << "  ";
      cout << setw(6) << it->second->get_contents()->size() << "  ";
      cout 
         << it->first 
         << it->second->get_contents()->dir_tail() 
         << endl;
      it++;
   }
}

//
// get_inode_ptr_from_path: 
//    returns the second-to-last file in given param "path"
//
inode_ptr& inode_state::get_inode_ptr_from_path(
   string path, string& tail)
{
   string front = "";
   if (path.size() > 0 && path.at(0) == '/') front = "/";
   auto files = split(path, "/");
   if (files.size() < 1) files.push_back("/");
   tail = files.back();
   size_t counter = 0;
   if (front == "/" || tail == "/") { 
      return this->get_root()->get_contents()->recur_get_dir(
         files, counter); 
      }
   else { 
      return this->get_cwd()->get_contents()->recur_get_dir(
         files, counter); }
}

inode_ptr& directory::recur_get_dir(wordvec& files, size_t counter) {
   try
   {
      if (counter < files.size() - 1) { 
         if (this->get_dirents().find(files.at(counter)) 
            == this->get_dirents().end()) { 
            throw file_error("Going to catch"); };
         return this->get_dirents()[files.at(counter)]
            ->get_contents()->recur_get_dir(files, counter + 1);
      }
      else  {
         return this->get_dirents()["."];
      }
   }
   catch(std::exception const& e) {
      throw file_error("Exiting recursive loop");
   }
}

void directory::recur_lsr() {

   map<string, inode_ptr>::const_iterator it = this->dirents.begin();
   this->print_dirents();
   while (it != this->dirents.end())
   {
      try { 
         if ( it->first != "." && it->first != "..")  {
            it->second->get_contents()->recur_lsr(); 
            }
         }
      catch(std::exception const& e) {}
      it++;
   }
}

void directory::rmr(string& filename) {
   try {
      if (this->dirents.find(filename) == this->dirents.end() 
      && filename != ".." && filename != "/") { 
         throw file_error("Null filename: Going to catch"); };
      this->dirents[filename]->get_contents()->recur_rmr();
      this->dirents[filename]->get_contents()
         ->get_dirents().erase(".");
      this->dirents[filename]->get_contents()
         ->get_dirents().erase("..");
      this->dirents[filename]->get_contents() = nullptr;
      this->dirents[filename] = nullptr;
      this->dirents.erase(filename);
   }
   catch(std::exception const& e) {
      this->remove(filename); // Handles null
   }
}

void directory::recur_rmr() {
   map<string, inode_ptr>::iterator it = this->dirents.begin();
   while (it != this->dirents.end()) {
      try { 
         if ( it->first != "." && it->first != "..")  {
            this->dirents[it->first]->get_contents()->recur_rmr();
            this->dirents[it->first]->get_contents() = nullptr;
         }
      }
      catch(std::exception const& e) {
         this->remove(it->first);
      }
      it++;
   }
}

inode_state::~inode_state() {
}
