#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <cstdlib>
#include <iostream>
#include <random>
#include <unordered_map>
#include <utility>

#include "main.h"
typedef std::vector<std::pair<double, std::string> > vector;
void skipSpace(const char ** strp);
void rephrase(std::string & str);
char * getop(const char ** strp);
double get_result(const char ** strp,
                  std::hash<std::string> & str_hash,
                  std::unordered_map<size_t, vector> & hash_map_func,
                  size_t hashNum,
                  std::unordered_map<size_t, std::string> & hash_map_rhs,
                  vector * v);

/* This file is used to implement the function of define.
 * After reading the "define" insrtuctions, the program will run this file's code,
 * save the function name, and the parameters, and the way this function computes into vector for future use.
 */

//
//
//
//

/* This function takes one parameter:
 * fname (the pointer to the string of function names)
 * and check if it's a valid function name
 * return 1 for valid, 0 for invalid.
 */
bool isValidOp(const char * fname) {
  //first, check if the function name is a build-in function
  if ((strcmp(fname, "sin") == 0) || (strcmp(fname, "cos") == 0) || (strcmp(fname, "ln") == 0) ||
      (strcmp(fname, "sqrt") == 0) || (strcmp(fname, "pow") == 0) || (strcmp(fname, "+") == 0) ||
      (strcmp(fname, "-") == 0) || (strcmp(fname, "*") == 0) || (strcmp(fname, "/") == 0) ||
      (strcmp(fname, "%") == 0)) {
    return true;
  }
  //second, check if the function name is consisted of pure letters
  while ((*fname) != '\0') {
    if (!isalpha(*fname)) {
      return false;
    }
    fname++;
  }
  return true;
}

/* This function takes two parameter:
 * v_name (the pointer to the string of parameter names)
 * f_v (the pointer to the vector which saves every parameter names inside)
 * and check if it's a valid parameter name
 * return 1 for valid, 0 for invalid.
 */
bool isValidVec(const char * v_name, vector * f_v) {
  std::string vector_name(v_name);
  //first, check if the parameter name is consisted of pure letters
  while ((*v_name) != '\0') {
    if (!isalpha(*v_name)) {
      return false;
    }
    v_name++;
  }
  //second, check if the parameter name is already been used before in this function through looking into the vector of parameter names
  size_t num = f_v->size();
  if (num == 0) {
    return true;
  }
  else {
    for (size_t i = 0; i < num; i++) {
      if ((*f_v)[i].second == vector_name) {
        return false;
      }
    }
    return true;
  }
}

/* This function takes two parameter:
 * strp (the pointer to the the pointer to a string that needs to be parsed)
 * f_v (the pointer to the vector which saves every parameter names inside)
 * and save one valid parameter name every time this function being called.
 */
void parse_vec(const char ** strp, vector * f_v) {
  skipSpace(strp);
  if (**strp == '\0') {
    std::cerr << "Invalid input format\n";
    exit(EXIT_FAILURE);
  }
  //meet the end of this part
  if (**strp == ')') {
    return;
  }
  //get a parameter name, check its validity, then save it, and call this function recursively
  else {
    char * name = getop(strp);
    if (!isValidVec(name, f_v)) {
      std::cerr << "Invalid id: " << name << "\n";
      free(name);
      exit(EXIT_FAILURE);
    }
    //before assign value to the parameters, we suppose their value are 1, for validity check (ln,/)
    double n = 1;
    std::string xid(name);
    std::pair<double, std::string> x1(n, xid);
    free(name);
    f_v->push_back(x1);
    parse_vec(strp, f_v);
  }
}

/* This function takes three parameter:
 * strp (the pointer to the the pointer to a string that needs to be parsed)
 * str_hash (the std::hash that used to compute the hash value for a std::string)
 * hash_map_func (the map that used to save function name's hash value and its vector which used to save its parameter names and values) 
 * and compute the hash value of the valid function name, then save it to the map
 * return the hash value.
 */
size_t parseOp_lhs(const char ** strp,
                   std::hash<std::string> & str_hash,
                   std::unordered_map<size_t, vector> & hash_map_func) {
  skipSpace(strp);
  char * fname = getop(strp);
  //check if fname valid
  if (!isValidOp(fname)) {
    std::cerr << "Invalid op: " << fname << "\n";
    free(fname);
    exit(EXIT_FAILURE);
  }
  if (**strp == ')') {
    free(fname);
    std::cerr << "No function name or parameter\n";
    exit(EXIT_FAILURE);
  }
  //get hash value of the fname
  std::string fid(fname);
  size_t hashNum = str_hash(fid);
  free(fname);
  vector f_v;
  //call parse_vec to save the parameters name after the function names
  parse_vec(strp, &f_v);
  std::pair<size_t, vector> f1(hashNum, f_v);
  //check if function name has already been defined
  std::unordered_map<size_t, vector>::iterator it = hash_map_func.find(hashNum);
  if (it == hash_map_func.end()) {
    hash_map_func.insert(f1);
  }
  else {
    std::cerr << "Same function name\n";
    exit(EXIT_FAILURE);
  }
  return hashNum;
}

/* This function takes three parameter:
 * strp (the pointer to the the pointer to a string that needs to be parsed)
 * str_hash (the std::hash that used to compute the hash value for a std::string)
 * hash_map_func (the map that used to save function name's hash value and its vector which used to save its parameter names and values) 
 * and start to parse the function name
 * return the function name's hash value.
 */
size_t parse_lhs(const char ** strp,
                 std::hash<std::string> & str_hash,
                 std::unordered_map<size_t, vector> & hash_map_func) {
  skipSpace(strp);
  if (**strp == '\0') {
    std::cerr << "End of line found mid expression!\n";
    exit(EXIT_FAILURE);
  }
  else if (**strp == '(') {
    // (op E E)
    *strp = *strp + 1;
    return parseOp_lhs(strp, str_hash, hash_map_func);
  }
  else {
    std::cerr << "Invalid format after ins\n";
    exit(EXIT_FAILURE);
  }
}

/* This function takes one parameter:
 * strp (the pointer to the the pointer to a string that needs to be parsed)
 * and get the rhs part after equal sign and before the pound sign
 * return that part of std::string 
 */
std::string save_rhs(const char ** strp) {
  skipSpace(strp);
  if (**strp != '=') {
    std::cerr << "Didn't find equal sign!\n";
    exit(EXIT_FAILURE);
  }
  *strp = *strp + 1;
  skipSpace(strp);
  std::string ans(*strp);
  //find #, and remove the after part
  if (ans.find('#') != std::string::npos) {
    size_t pos = ans.find('#');
    ans = ans.substr(0, pos);
  }
  return ans;
}

/* This function takes three parameter:
 * strp (the pointer to the the pointer to a string that is rhs of the equal expression)
 * str_hash (the std::hash that used to compute the hash value for a std::string)
 * hash_map_func (the map that used to save function name's hash value and its vector which used to save its parameter names and values) 
 * and check if the function name of rhs is defined before.
 * if not, exit failure.
 */
void check_rhs_op_isdefined(std::string rhs,
                            std::hash<std::string> & str_hash,
                            std::unordered_map<size_t, vector> & hash_map_func) {
  char * temp = strdup(rhs.c_str());
  const char * strp = temp;
  while ((*strp) != '\0') {
    if ((*strp) == '(') {
      strp++;
      skipSpace(&strp);
      char * fname = getop(&strp);
      std::string f_name(fname);
      free(fname);
      size_t hashNum = str_hash(f_name);
      std::unordered_map<size_t, vector>::iterator it = hash_map_func.find(hashNum);
      if (it == hash_map_func.end()) {
        std::cerr << "Function undefined: " << f_name << "\n";
        exit(EXIT_FAILURE);
      }
    }
    strp++;
  }
  free(temp);
}

/* This function takes six parameter:
 * temp (the pointer to a string that needs to be parsed)
 * mid (the pointer used to mark the position of the middle of the string)
 * lhs (the string that reads)
 * str_hash (the std::hash that used to compute the hash value for a std::string)
 * hash_map_func (the map that used to save function name's hash value and its vector which used to save its parameter names and values) 
 * hash_map_rhs (the map that used to save function name's hash value and its rhs of the define instructions)
 * and print out the function you defined with its function name and parameter name.
 */
void define(const char * temp,
            const char * mid,
            std::string lhs,
            std::hash<std::string> & str_hash,
            std::unordered_map<size_t, vector> & hash_map_func,
            std::unordered_map<size_t, std::string> & hash_map_rhs) {
  //make find_equal point to the position after =
  const char * find_equal = temp;
  while ((*find_equal) != '=') {
    if ((*find_equal) == '\0') {
      std::cerr << "Didn't find equal sign!\n";
      exit(EXIT_FAILURE);
    }
    find_equal++;
  }
  //save rhs into a std::string
  std::string rhs = save_rhs(&find_equal);
  //check if the function name of the rhs is defined before
  check_rhs_op_isdefined(rhs, str_hash, hash_map_func);
  size_t hashNum = parse_lhs(&temp, str_hash, hash_map_func);
  temp += 1;
  //cut the lhs string to get the real lhs part, which is before the equal sign
  size_t len_def = temp - mid;
  lhs = lhs.substr(0, len_def);
  //change it's order into a standard format to print
  rephrase(lhs);
  //save rhs into hash_map_rhs, used in the future when test instruction needs to get a result
  std::pair<size_t, std::string> rhs_string(hashNum, rhs);
  hash_map_rhs.insert(rhs_string);
  std::unordered_map<size_t, vector>::iterator it = hash_map_func.find(hashNum);
  if (it == hash_map_func.end()) {
    std::cerr << "Function undefined\n";
    exit(EXIT_FAILURE);
  }
  //check the grammer of the rhs using the supposed value 1 of parameters, don't need to return a real result, but as long as the get_result function don't exit, the grammer is good
  vector v = it->second;
  get_result(&find_equal, str_hash, hash_map_func, hashNum, hash_map_rhs, &v);
  std::cout << "defined " << lhs << "\n";
}
