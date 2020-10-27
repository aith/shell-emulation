// $Id: file_sys.cpp,v 1.8 2020-10-22 14:37:26-07 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>

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

// TODO
inode_state::inode_state() {
   // make_shared does allocate space for the object it points to bc of ctor
   root = make_shared<inode>(file_type::DIRECTORY_TYPE);  // public ctor
   cwd = root;
   root->contents->get_dirents().insert(pair<string, inode_ptr>(".",root));
   root->contents->get_dirents().insert(pair<string, inode_ptr>("..",root));
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

// Note how the param is an enum
inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      // TIL you can set a pointer to a parent from the pointer of a subclass
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>(); 
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>(); //default ctor... an empty dir??
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


// TODO
 // Iterate through the vector, find all the lengths of the strings in the vector, and assign it to size... rn its always 0
size_t plain_file::size() const { 
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

// This is done
const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

// TODO
void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
}

// TODO num of entried in dir
size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   return size;
}

// TODO remove file
void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
}

// TODO. need to add . and .. here.
inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   inode_ptr dir = make_shared<inode>(file_type::DIRECTORY_TYPE);  // public ctor
   string parent_path = this->get_path(); // the current instance of dir
   string new_path = parent_path + dirname + "/";
   dir->get_contents()->set_path(new_path);
   this->dirents.insert(pair<string, inode_ptr>(dirname,dir));
   dir->get_contents()->get_dirents().insert(pair<string, inode_ptr>(".",dir));
   dir->get_contents()->get_dirents().insert(pair<string, inode_ptr>("..",this->get_dirents().at(".")));
   return dir;
}

// TODO enters a new file in dir
inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   return nullptr;
}

void directory::print_dirents() const {
   map<string, inode_ptr>::const_iterator it = this->dirents.begin();
   while (it != this->dirents.end())
   {
      // TODO size
      // TODO spacing
      cout << it->second->get_inode_nr() << " ";
       cout << it->first << endl;
       // Increment the Iterator to point to next entry
       it++;
   }
}