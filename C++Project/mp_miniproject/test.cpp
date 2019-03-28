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
double parseOp_t(const char ** strp,
                 std::hash<std::string> & str_hash,
                 std::unordered_map<size_t, vector> & hash_map_func,
                 std::unordered_map<size_t, std::string> & hash_map_rhs);
void skipSpace(const char ** strp);
char * getop(const char ** strp);
Expression * parse(const char ** strp,
                   std::hash<std::string> & str_hash,
                   std::unordered_map<size_t, vector> & hash_map_func,
                   size_t hashNum,
                   std::unordered_map<size_t, std::string> & hash_map_rhs,
                   vector * v);
bool isValidOp(const char * fname);
void rephrase(std::string & str);
bool check_end(const char ** strp);
double parse_t(const char ** strp,
               std::hash<std::string> & str_hash,
               std::unordered_map<size_t, vector> & hash_map_func,
               std::unordered_map<size_t, std::string> & hash_map_rhs);

/* This file is used to test the pre-defined functions and compare the result to get a conclusion, whether correct or incorrect.
 * This file code assign values to the parameters, and then get a result of the calculation of the rhs of the define instructions.
 * The comparition can be used between expressions to expressions, constant number to constant number, or expressions to constant number
 */

//
//
//
//

/* This function takes two parameter:
 * n (the result got from strtod)
 * name (the pointer to the string of numbers)
 * and check if this string is a valid number.
 * return 1 for valid, 0 for invalid.
 */
bool check_digit(double n, char * name) {
  //strtod will return 0 even if it reads a letter, but when it return other than, means it's valid numbers
  if (n == 0) {
    //rule out 2 valid instances, just return 1
    if (strcmp(name, "0.000000") == 0 || strcmp(name, "-0.000000") == 0) {
      return true;
    }
    //check every char in the string, make sure they are all digit
    if (isdigit(*name)) {
      name++;
      if ((*name) != '\0') {
        return false;
      }
    }
    else {
      return false;
    }
  }
  return true;
}

/* This function takes two parameter:
 * hashNum (the hash value of the string of function names)
 * hash_map_rhs (the map that saves hash value for function name and the string for rhs of define instructions)
 * and return the pointer to the rhs string
 */
char * get_ptr(size_t hashNum, std::unordered_map<size_t, std::string> & hash_map_rhs) {
  std::unordered_map<size_t, std::string>::iterator it = hash_map_rhs.find(hashNum);
  if (it == hash_map_rhs.end()) {
    std::cerr << "Function undefined\n";
    exit(EXIT_FAILURE);
  }
  std::string rhs = it->second;
  return strdup(rhs.c_str());
}

/* This function takes six parameter:
 * strp (the pointer to the pointer to a string that needs to be parsed)
 * str_hash (the std::hash that used to compute the hash value for a std::string)
 * hash_map_func (the map that used to save function name's hash value and its vector which used to save its parameter names and values) 
 * hashNum (the hash value for the string of function name)
 * hash_map_rhs (the map that used to save function name's hash value and its rhs of the define instructions)
 * v (the pointer to the vector which consists of parameter names and values of the function)
 * and return the calculation result of the rhs of define instructions expression.
 */
double get_result(const char ** strp,
                  std::hash<std::string> & str_hash,
                  std::unordered_map<size_t, vector> & hash_map_func,
                  size_t hashNum,
                  std::unordered_map<size_t, std::string> & hash_map_rhs,
                  vector * v) {
  Expression * expr = parse(strp, str_hash, hash_map_func, hashNum, hash_map_rhs, v);
  //check if there is some trash after the expression
  if (!check_end(strp)) {
    std::cerr << "Jibber jabber at the end.\n";
    exit(EXIT_FAILURE);
  }
  if (expr != NULL) {
    double ans = expr->evaluate();
    delete expr;
    return ans;
  }
  else {
    std::cout << "Could not parse expression, please try again.\n";
    exit(EXIT_FAILURE);
  }
}

/* This function takes seven parameter:
 * strp (the pointer to the pointer to a string that needs to be parsed)
 * v (the pointer to the vector which consists of parameter names and values of the function)
 * i (used to count the num of parameter in the expression, for checking if the num is correct)
 * str_hash (the std::hash that used to compute the hash value for a std::string)
 * hash_map_func (the map that used to save function name's hash value and its vector which used to save its parameter names and values) 
 * hashNum (the hash value for the string of function name)
 * hash_map_rhs (the map that used to save function name's hash value and its rhs of the define instructions)
 * and parse the expression after the function name.
 * return the calculation result of the get_result function.
 */
double parse_t_vec(const char ** strp,
                   vector * v,
                   size_t & i,
                   std::hash<std::string> & str_hash,
                   std::unordered_map<size_t, vector> & hash_map_func,
                   size_t hashNum,
                   std::unordered_map<size_t, std::string> & hash_map_rhs) {
  skipSpace(strp);
  if (**strp == '\0') {
    std::cerr << "Invalid input format\n";
    exit(EXIT_FAILURE);
  }
  if (**strp == ')') {
    *strp = *strp + 1;
    //do the evaluation
    char * ptr = get_ptr(hashNum, hash_map_rhs);
    const char * ptr_const = ptr;

    double ans = get_result(&ptr_const, str_hash, hash_map_func, hashNum, hash_map_rhs, v);
    free(ptr);
    return ans;
  }
  if (**strp == '(') {
    //went back to parse_t recursively
    double n = parse_t(strp, str_hash, hash_map_func, hash_map_rhs);
    (*v)[i].first = n;
    i++;
    return parse_t_vec(strp, v, i, str_hash, hash_map_func, hashNum, hash_map_rhs);
  }
  else {
    //deal with the parameter values
    char * name = getop(strp);
    if ((*v).size() < i + 1) {
      std::cerr << "More nums than expected!\n";
      exit(EXIT_FAILURE);
    }
    char * endp;
    double n = strtod(name, &endp);
    if ((!check_digit(n, name)) || (*endp) != '\0' || (*(--endp)) == '.') {
      std::cerr << "The parameter :" << name << " is not a double number\n";
      exit(EXIT_FAILURE);
    }
    (*v)[i].first = n;
    free(name);
    i++;
    return parse_t_vec(strp, v, i, str_hash, hash_map_func, hashNum, hash_map_rhs);
  }
}

/* This function takes seven parameter:
 * strp (the pointer to the pointer to a string that needs to be parsed)
 * str_hash (the std::hash that used to compute the hash value for a std::string)
 * hash_map_func (the map that used to save function name's hash value and its vector which used to save its parameter names and values)
 * hash_map_rhs (the map that used to save function name's hash value and its rhs of the define instructions)
 * and parse the expression start from the function name.
 * return the calculation result of the parse_t_vec function.
 */
double parseOp_t(const char ** strp,
                 std::hash<std::string> & str_hash,
                 std::unordered_map<size_t, vector> & hash_map_func,
                 std::unordered_map<size_t, std::string> & hash_map_rhs) {
  skipSpace(strp);
  char * fname = getop(strp);
  //check if function name valid
  if (!isValidOp(fname)) {
    std::cerr << "Invalid op: " << fname << "\n";
    free(fname);
    exit(EXIT_FAILURE);
  }
  skipSpace(strp);
  if (**strp == ')') {
    free(fname);
    std::cerr << "No parameter\n";
    exit(EXIT_FAILURE);
  }
  //get hash value
  std::string fid(fname);
  size_t hashNum = str_hash(fid);
  free(fname);
  //find its vector, make a deep copy, and send its pointer to parse_t_vec
  std::unordered_map<size_t, vector>::iterator it = hash_map_func.find(hashNum);
  if (it == hash_map_func.end()) {
    std::cerr << "Function undefined: " << fid << "\n";
    exit(EXIT_FAILURE);
  }
  vector v;
  for (size_t j = 0; j < it->second.size(); j++) {
    v.push_back(it->second[j]);
  }
  //parse_v
  size_t i = 0;
  double ans = parse_t_vec(strp, &v, i, str_hash, hash_map_func, hashNum, hash_map_rhs);
  if (v.size() > i) {
    std::cerr << "Less nums than expected!\n";
    exit(EXIT_FAILURE);
  }
  return ans;
}

/* This function takes seven parameter:
 * strp (the pointer to the pointer to a string that needs to be parsed)
 * str_hash (the std::hash that used to compute the hash value for a std::string)
 * hash_map_func (the map that used to save function name's hash value and its vector which used to save its parameter names and values)
 * hash_map_rhs (the map that used to save function name's hash value and its rhs of the define instructions)
 * and parse the expression start from '('.
 * return the calculation result of the parseOp_tt function.
 */
double parse_t(const char ** strp,
               std::hash<std::string> & str_hash,
               std::unordered_map<size_t, vector> & hash_map_func,
               std::unordered_map<size_t, std::string> & hash_map_rhs) {
  skipSpace(strp);
  if (**strp == '\0') {
    std::cerr << "End of line found mid expression!\n";
    exit(EXIT_FAILURE);
  }
  else if (**strp == '(') {
    // (op E E)
    *strp = *strp + 1;
    return parseOp_t(strp, str_hash, hash_map_func, hash_map_rhs);
  }
  else {
    //this is a special case that the test object is not an expression, but a constant number
    //then read the number, check its validity, and return its value
    char * name = getop(strp);
    char * endp;
    double n = strtod(name, &endp);
    if ((!check_digit(n, name)) || (*endp) != '\0' || (*(--endp)) == '.') {
      std::cerr << "The result: " << name << " is not a double number\n";
      free(name);
      exit(EXIT_FAILURE);
    }
    free(name);
    return n;
  }
}

/* this function defines the definition of equal
 */
bool test_equal(double x, double y) {
  double sub = std::abs(x - y);
  if (sub < 0.0000000000001) {
    return true;
  }
  else {
    return false;
  }
}

/* This function takes six parameter:
 * temp (the pointer to a string that needs to be parsed)
 * mid (the pointer used to mark the position of the middle of the string)
 * lhs (the string that reads)
 * str_hash (the std::hash that used to compute the hash value for a std::string)
 * hash_map_func (the map that used to save function name's hash value and its vector which used to save its parameter names and values) 
 * hash_map_rhs (the map that used to save function name's hash value and its rhs of the define instructions)
 * and print out the test result with the correct format.
 */
void test(const char * temp,
          const char * mid,
          std::string lhs,
          std::hash<std::string> & str_hash,
          std::unordered_map<size_t, vector> & hash_map_func,
          std::unordered_map<size_t, std::string> & hash_map_rhs) {
  //get the first expression or constant number
  double ans_1 = parse_t(&temp, str_hash, hash_map_func, hash_map_rhs);
  temp += 1;
  size_t len_t = temp - mid;
  lhs = lhs.substr(0, len_t);
  rephrase(lhs);
  //get the second expression or constant number
  skipSpace(&temp);
  std::string t_rhs(temp);
  rephrase(t_rhs);
  double ans_2 = parse_t(&temp, str_hash, hash_map_func, hash_map_rhs);
  //check if there are trash after expression
  if (!check_end(&temp)) {
    std::cerr << "Jibber jabber at the end.\n";
    exit(EXIT_FAILURE);
  }
  // test_equal(ans_1, ans_2) and print;
  int i = test_equal(ans_1, ans_2);
  if (i == 1) {
    std::cout << lhs << "= " << ans_2 << " [correct]\n";
  }
  else {
    std::cout << lhs << "= " << ans_2 << " [INCORRECT: expected " << ans_1 << "]\n";
  }
}
