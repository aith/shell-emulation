// $Id: file_sys.h,v 1.8 2020-10-22 14:37:26-07 - - $

#ifndef __INODE_H__
#define __INODE_H__

#include <exception>
#include <iostream>
#include <memory>
#include <map>
#include <vector>
using namespace std;

#include "util.h"

// inode_t -
//    An inode is either a directory or a plain file.

enum class file_type {PLAIN_TYPE, DIRECTORY_TYPE};
class inode;
class base_file;
class plain_file;
class directory;
using inode_ptr = shared_ptr<inode>;
using base_file_ptr = shared_ptr<base_file>;
ostream& operator<< (ostream&, file_type);


// inode_state -
//    A small convenient class to maintain the state of the simulated
//    process:  the root (/), the current directory (.), and the
//    prompt.

class inode_state {
   friend class inode;
   friend ostream& operator<< (ostream& out, const inode_state&);
   private:
      inode_ptr root {nullptr}; // TODO Setter TODO Getter... for every variable is this asg
      inode_ptr cwd {nullptr};  // TODO Setter TODO Getter
      string prompt_ {"% "};
   public:
      inode_state (const inode_state&) = delete; // copy ctor
      inode_state& operator= (const inode_state&) = delete; // op=
      inode_state();  // ctor
      const string& prompt() const;  // getter
      void prompt (const string&);   // setter

      inode_ptr& get_cwd() { return cwd; }
      void set_cwd(inode_ptr& new_cwd) { this->cwd = new_cwd; }
      inode_ptr& get_root() { return root; }

      inode_ptr& get_inode_ptr_from_path(string, string&);
};

// class inode -
// inode ctor -
//    Create a new inode of the given type.
// get_inode_nr -
//    Retrieves the serial number of the inode.  Inode numbers are
//    allocated in sequence by small integer.
// size -
//    Returns the size of an inode.  For a directory, this is the
//    number of dirents.  For a text file, the number of characters
//    when printed (the sum of the lengths of each word, plus the
//    number of words.
//    

class inode {
   friend class inode_state;
   private:
      static size_t next_inode_nr;
      size_t inode_nr;
      base_file_ptr contents;
   public:
      inode (file_type);  // declare ctor
      size_t get_inode_nr() const;
      base_file_ptr& get_contents() {
         return contents;
      };
};


// class base_file -
// Just a base class at which an inode can point.  No data or
// functions.  Makes the synthesized members useable only from
// the derived classes.

class file_error: public runtime_error {
   public:
      explicit file_error (const string& what);
};

class base_file {
   protected:
      base_file() = default;
      virtual const string& error_file_type() const = 0;
   public:
      virtual ~base_file() = default;
      base_file (const base_file&) = delete;
      base_file& operator= (const base_file&) = delete;
      virtual size_t size() const = 0;
      virtual const wordvec& readfile() const;
      virtual void writefile (const wordvec& newdata);
      virtual void remove (const string& filename);
      virtual inode_ptr mkdir (const string& dirname);
      virtual inode_ptr mkfile (const string& filename);

      // Creates a base-case, that throws error if subclass doesn't implement
      virtual map<string,inode_ptr>& get_dirents() {
         throw file_error ("is a " + error_file_type()); };
      virtual string& get_path() { 
         throw file_error ("is a " + error_file_type()); };
      virtual void set_path(const string&) { 
         throw file_error ("is a " + error_file_type()); };
      virtual void print_dirents() const { 
         throw file_error ("is a " + error_file_type()); };
      virtual string dir_tail() const { 
         throw file_error ("is a " + error_file_type()); };
      virtual inode_ptr& recur_get_dir(wordvec&, size_t) {
         throw file_error ("is a " + error_file_type()); };
      virtual void recur_lsr() {
         throw file_error ("is a " + error_file_type()); };
      virtual void rmr(string&) {
         throw file_error ("is a " + error_file_type()); };
      virtual void recur_rmr() {
         throw file_error ("is a " + error_file_type()); };
};

// class plain_file -
// Used to hold data.
// synthesized default ctor -
//    Default vector<string> is a an empty vector.
// readfile -
//    Returns a copy of the contents of the wordvec in the file.
// writefile -
//    Replaces the contents of a file with new contents.

class plain_file: public base_file {
   private:
      wordvec data;
      virtual const string& error_file_type() const override {
         static const string result = "plain file";
         return result;
      }
      string path; // now create a getter and sette
   public:
      virtual size_t size() const override;
      // These are the only 2 things you can do to a plain_file
      virtual const wordvec& readfile() const override;         
      virtual void writefile (const wordvec& newdata) override;
      virtual void set_path(const string& filepath) override { this->path = filepath; };
      virtual string dir_tail() const override { return ""; };
      virtual string& get_path() override { return path; };
};

// class directory -
// Used to map filenames onto inode pointers.
// default ctor -
//    Creates a new map with keys "." and "..".
// remove -
//    Removes the file or subdirectory from the current inode.
//    Throws an file_error if this is not a directory, the file
//    does not exist, or the subdirectory is not empty.
//    Here empty means the only entries are dot (.) and dotdot (..).
// mkdir -
//    Creates a new directory under the current directory and 
//    immediately adds the directories dot (.) and dotdot (..) to it.
//    Note that the parent (..) of / is / itself.  It is an error
//    if the entry already exists.
// mkfile -
//    Create a new empty text file with the given name.  Error if
//    a dirent with that name exists.

class directory: public base_file { // Just a map
   private:
      // Must be a map, not unordered_map, so printing is lexicographic
      map<string,inode_ptr> dirents; 
                                     
      virtual const string& error_file_type() const override {
         static const string result = "directory";
         return result;
      }
      string path; // now create a getter and sette
   public:
      virtual size_t size() const override;
      virtual void remove (const string& filename) override;
      virtual inode_ptr mkdir (const string& dirname) override;
      virtual inode_ptr mkfile (const string& filename) override;
      // map.insert(pair) // make a setter func with this code
      // TODO: Does this return the reference? How?
      virtual map<string,inode_ptr>& get_dirents() override {
         return dirents; };
      virtual string& get_path() override { return path; };
      virtual void set_path(const string& filepath) override { this->path = filepath; };
      virtual void print_dirents() const override;
      virtual string dir_tail() const override { return "/"; };
      virtual inode_ptr& recur_get_dir(wordvec& files, size_t counter) override;
      virtual void recur_lsr() override;
      virtual void rmr(string&) override;
      virtual void recur_rmr() override;
};

#endif


