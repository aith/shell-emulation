// $Id: commands.cpp,v 1.19 2020-10-20 18:23:13-07 - - $

#include "commands.h"
#include "debug.h"
#include <iomanip>      // std::setw

command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rmr   },
   {"#"     , fn_ignore},
   {"^D"    , fn_exit},
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int status = exec::status();
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}

void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state); DEBUGF ('c', words);
   string filename = "";
   string err = "No such plain file.";
   for (size_t file_num = 1; file_num < words.size(); ++file_num){
      if (words.size() < 2) { cout << err << endl; continue; }
      err = "cat: " + words.at(1) + ": " + err;
      try { 
         auto dir = state.get_inode_ptr_from_path(
            words.at(file_num), filename);
         auto dirents = dir->get_contents()->get_dirents();
         if (dirents.find(filename) == dirents.end()) {
            throw file_error("Going to catch"); };
         auto toCat = dir->get_contents()->get_dirents()[filename];
         for ( auto i : toCat->get_contents()->readfile()) {
            cout << i << " "; }
         cout << endl;
      }
      catch(std::exception const& e) {
         cout << err << endl; }
   }
}

void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state); DEBUGF ('c', words);
   auto err = "Please specify directory name. No plain files.";
   string dirname = "";
   if (words.size() < 2) { state.set_cwd(state.get_root()); return; }
   try {
      auto dir = state.get_inode_ptr_from_path(words.at(1), dirname);
      auto dirents = dir->get_contents()->get_dirents();
      if (dirname == "/") { state.set_cwd(state.get_root()); return; }
      if (dirents.find(dirname) == dirents.end()) {
         throw file_error("Going to catch"); };
      auto toCd = dirents[dirname];
      toCd->get_contents()->get_dirents(); // Verify its a dir
      state.set_cwd(toCd);
   }
   catch(std::exception const& e) {
      cout << err << endl; return; }
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state); DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state); DEBUGF ('c', words);
   int val = 0;
   if (words.size() > 1) {
      try {
         val = std::stoi(words.at(1));
      } 
      catch(std::exception const& e) {
         val = 127;
      }
   }
   exec::status(val);

   state.get_cwd() = state.get_root();  // Let's recursively clear state
   state.get_root()->get_contents()->recur_rmr();
   state.get_root()->get_contents()->get_dirents().erase(".");
   state.get_root()->get_contents()->get_dirents().erase("..");
   state.get_root()->get_contents() = nullptr;
   state.get_root() = nullptr;
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state); DEBUGF ('c', words);
   if (words.size() < 2) { 
      state.get_cwd()->get_contents()->print_dirents(); return; }
   for (size_t path_num = 1; path_num < words.size(); ++path_num){
      // auto err = "Please specify directory name. No plain files.";
      auto dir = state.get_cwd();
      string dirname = "";
      try {
         dir = state.get_inode_ptr_from_path(
            words.at(path_num), dirname);
         auto dirents = dir->get_contents()->get_dirents();
         if (dirname == "/") {
            state.get_root()->get_contents()->print_dirents(); 
            continue; 
            }
         if (dirents.find(dirname) == dirents.end()) {
            throw file_error("Going to catch"); };
         auto toLs = dirents[dirname];
         toLs->get_contents()->get_dirents(); // Verify its a dir
         toLs->get_contents()->print_dirents();
      }
      catch(std::exception const& e) {
         if (dir->get_contents()->get_dirents().find(dirname) !=
         dir->get_contents()->get_dirents().end() ) {
            cout << setw(6) << dir->get_contents()->
               get_dirents()[dirname]->get_inode_nr();
            cout << setw(8) << dir->get_contents()->
               get_dirents()[dirname]->get_contents()->size() << "  ";
            cout << dirname << endl; continue; 
         }
         else cout << "File does not exist." << endl;
      }
   }
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state); DEBUGF ('c', words);
   if (words.size() < 2) { 
      state.get_cwd()->get_contents()->print_dirents(); return; }
   for (size_t path_num = 1; path_num < words.size(); ++path_num){
      auto err = "Please specify directory name. No plain files.";
      auto dirToLsr = state.get_root();
      auto dir = state.get_cwd();
      if (words.size() > 1) {
         string dirname = "";
         try {
            dir = state.get_inode_ptr_from_path(
               words.at(path_num), dirname);
            auto dirents = dir->get_contents()->get_dirents();
            if (dirname == "/") { dirname = "."; }
            if (dirents.find(dirname) == dirents.end()) {
               throw file_error("Going to catch"); };
            dirToLsr = dirents[dirname];
            dirToLsr->get_contents()->recur_lsr();
         }
         catch(std::exception const& e) {
         if (dir->get_contents()->get_dirents().find(dirname) !=
         dir->get_contents()->get_dirents().end() ) {
            cout << setw(6) << dir->get_contents()
               ->get_dirents()[dirname]->get_inode_nr() << "  ";
            cout << setw(6) << dir->get_contents()
               ->get_dirents()[dirname]->get_contents()->size() << "  ";
            cout << dirname << endl; continue; 
         }
         else 
            { cout << err << endl; continue; }
         }
      }
      else dirToLsr->get_contents()->recur_lsr();
   }
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state); DEBUGF ('c', words);
   auto err = "Please specify file name. No directories.";
   if (words.size() < 2) { cout << err << endl; return; }
   try
   {
      string back_name = "";
      auto toMake = state.get_inode_ptr_from_path(
         words.at(1), back_name);
      auto existing_file_dirents = toMake->get_contents()
         ->get_dirents();
      if(existing_file_dirents.find(back_name) == 
         existing_file_dirents.end()) 
      { 
         toMake->get_contents()->mkfile(back_name)
            ->get_contents()->writefile(words); }
      else { existing_file_dirents[back_name]->get_contents()
            ->writefile(words); }
   }
   catch(std::exception const& e) {
      cout << err << endl;
   }
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state); DEBUGF ('c', words);
   string back_name = "";
   try {
      auto toMakeIn = state.get_inode_ptr_from_path(
         words.at(1), back_name);
      auto dirents = toMakeIn->get_contents()->get_dirents();
      if (dirents.find(back_name) == dirents.end()) { 
         toMakeIn->get_contents()->mkdir(back_name);
      }
      else { cout << "Directory already exists." << endl; };
   }
   catch(std::exception const& e) {
      cout << "Directory path does not exist." << endl; 
   }
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state); DEBUGF ('c', words);
   string concat = "";
   for (size_t i = 1; i < words.size(); i++) { 
      concat += words.at(i); 
      concat += " ";
   }
   if (concat ==  "") concat = " ";
   state.prompt(concat);
}

void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state); DEBUGF ('c', words);
   auto toPrint = state.get_cwd()->get_contents()->get_path();
   if (state.get_cwd() != state.get_root()) { 
      toPrint = toPrint.substr(0, toPrint.size()-1); }
   cout << toPrint << endl;
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state); DEBUGF ('c', words);
   auto err = "File does not exist.";
   string toDelete = "";
   try {
      auto toDeleteFrom = state.get_inode_ptr_from_path(
         words.at(1), toDelete);
      if (toDelete == ".." || toDelete == "/") 
         { throw file_error("Going to catch"); }
      if (state.get_cwd() == 
         toDeleteFrom->get_contents()->get_dirents()[toDelete] ) 
      {
         cout << "Cannot delete pwd." << endl;
         return;
      }
      toDeleteFrom->get_contents()->remove(toDelete); 
   }
   catch(std::exception const& e) {
      cout << err << endl; return; }
   // 
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state); DEBUGF ('c', words);
   auto err = "Directory path does not exist.";
   string toDelete = "";
   try {
      auto toDeleteFrom = state.get_inode_ptr_from_path(
         words.at(1), toDelete);
      if (toDelete == ".." || toDelete == "/") 
         { throw file_error("Going to catch"); }
      if (state.get_cwd() == 
         toDeleteFrom->get_contents()->get_dirents()[toDelete] ) 
      {
         cout << "Cannot delete pwd." << endl;
         return;
      }   
      toDeleteFrom->get_contents()->rmr(toDelete); 
      }
   catch(std::exception const& e) {
      cout << err << endl; return; }

}

void fn_ignore (inode_state& state, const wordvec& words){
   return;
}
   

