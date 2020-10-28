// $Id: commands.cpp,v 1.19 2020-10-20 18:23:13-07 - - $

#include "commands.h"
#include "debug.h"

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
   // DONE
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string filename = "";
   string err = "No such file or directory";
   if (words.size() < 2) { cout << err << endl; return; }
   err = "cat: " + words.at(1) + ": " + err;
   try { 
      auto dir = state.get_inode_ptr_from_path(words.at(1), filename);
      auto toCat = dir->get_contents()->get_dirents()[filename];
      for ( auto i : toCat->get_contents()->readfile()) {
         cout << i << endl; }
   }
   catch(std::exception const& e) {
      cout << err << endl; return; }
}

void fn_cd (inode_state& state, const wordvec& words){
   // DONEa
   auto err = "Please specify directory name. No plain files.";
   string dirname = "";
   if (words.size() < 2) { state.set_cwd(state.get_root()); return; }
   auto dir = state.get_inode_ptr_from_path(words.at(1), dirname);
   auto toCd = dir->get_contents()->get_dirents()[dirname];
   try {
      state.set_cwd( toCd);
   }
   catch(std::exception const& e) {
      cout << err << endl; return; }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
   // DONE
   auto err = "Please specify directory name. No plain files.";
   string dirname = "";
   if (words.size() < 2) { 
      state.get_cwd()->get_contents()->print_dirents(); return; }
   auto dir = state.get_inode_ptr_from_path(words.at(1), dirname);
   auto toLs = dir->get_contents()->get_dirents()[dirname];
   try {
      // Error check that it's a directory. Add this to all of the above
      toLs = toLs->get_contents()->get_dirents()["."];

      toLs->get_contents()->print_dirents();
   }
   catch(std::exception const& e) {
      cout << err << endl; return; }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_lsr (inode_state& state, const wordvec& words){
   auto err = "Please specify directory name. No plain files.";
   auto dirToLsr = state.get_root();
   if (words.size() > 1) {
      string dirname = "";
      try {
         auto dir = state.get_inode_ptr_from_path(words.at(1), dirname);
         dirToLsr = dir->get_contents()->get_dirents()[dirname];
         // Error check that it's a directory. 
         dirToLsr = dirToLsr->get_contents()->get_dirents()["."];
      }
      catch(std::exception const& e) {
         cout << err << endl; return; }
   }
   dirToLsr->get_contents()->recur_lsr();
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words){
   // DONE
   auto err = "Please specify file name. No directories.";
   if (words.size() < 2) { cout << err << endl; return; }
   try
   {
      string back_name = "";
      auto toMake = state.get_inode_ptr_from_path(words.at(1), back_name);
      auto existing_file_dirents = toMake->get_contents()
         ->get_dirents();
      // auto back_name = split(words.at(1), "/").back();
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

   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_mkdir (inode_state& state, const wordvec& words){
   string back_name = "";
   auto toMakeIn = state.get_inode_ptr_from_path(words.at(1), back_name);
   auto existing_file_dirents = toMakeIn->get_contents() ->get_dirents();
   if(existing_file_dirents.find(back_name) == 
      existing_file_dirents.end()) 
   {
   // TODO parse path
   // TODO check duplicates and then err
   state.get_cwd()->get_contents()->mkdir(words.at(1));
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_pwd (inode_state& state, const wordvec& words){
   auto toPrint = state.get_cwd()->get_contents()->get_path();
   if (state.get_cwd() != state.get_root()) { 
      toPrint = toPrint.substr(0, toPrint.size()-1); }
   cout << toPrint << endl;
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

