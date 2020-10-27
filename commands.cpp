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
   // TODO parsing
   string err = "No such file or directory";
   if (words.size() > 2) { cout << err << endl; return; }
   err = "cat: " + words.at(1) + ": " + err;
   // auto entry = state.get_cwd()->get_contents()->get_dirents()[words.at(1)];
   auto map = state.get_cwd()->get_contents()->get_dirents();
   if (map.find(words.at(1)) == map.end()) { cout << err << endl; return; }
   try {
      for ( auto i : map[words.at(1)]->get_contents()->readfile()) 
      {
         cout << i << endl; 
      }
   }
   // is this the safest version?
   catch(std::exception const& e) {
      cout << err << endl;
   }
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_cd (inode_state& state, const wordvec& words){
   // TODO check for words
   // TODO check if plain_file
   state.set_cwd(
      state.get_cwd()->get_contents()->get_dirents()[words.at(1)]
   );
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_echo (inode_state& state, const wordvec& words){
   // TODO is this done??
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
   // for all files in dirent, cout them
   // creating print_dirents so I can access them easier
   // TODO parse
   state.get_cwd()->get_contents()->print_dirents();
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_make (inode_state& state, const wordvec& words){
   // TODO check for name dupe
   // TODO check for not long enough entry
   // TODO rewrite over file doesnt work

   // use mkfile w words.at(0)
   // use writefile on that inodeptr
   // Clever! mkfile returns a ptr to new file
   state.get_cwd()->get_contents()->mkfile(words.at(1))
      ->get_contents()->writefile(words);

   // before adding data to that file w writedata, lets see if we can allocate this
   // state.get_cwd()->get_contents()->writefile(words);
   // wordvec::const_iterator it = words.begin();
   // while (it != words.end())
   // {
   //    // cat it to data. smth like
   //    data.push_back(*it);
   //    cout << *it << endl;
   //    it++;
   // }
   // state.getcwd()->get_contents()->writefile(words)
   DEBUGF ('c', state);
   DEBUGF ('c', words);
}

void fn_mkdir (inode_state& state, const wordvec& words){
   // is this right? the base file taht inode-ptr points to
   // the pointers themselves dont have the get commands, 
   // so you use get from what they point to
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
   cout << state.get_cwd()->get_contents()->get_path() << endl;
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

