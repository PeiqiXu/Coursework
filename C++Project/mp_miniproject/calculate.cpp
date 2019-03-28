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
bool check_end(const char ** strp);
bool check_digit(double n, char * name);
double parse_t(const char ** strp,
               std::hash<std::string> & str_hash,
               std::unordered_map<size_t, vector> & hash_map_func,
               std::unordered_map<size_t, std::string> & hash_map_rhs);
Expression * parse(const char ** strp,
                   std::hash<std::string> & str_hash,
                   std::unordered_map<size_t, vector> & hash_map_func,
                   size_t hashNum,
                   std::unordered_map<size_t, std::string> & hash_map_rhs,
                   vector * v);

/* This file is used to mainly for the calculation of the expression that parsed in by define and assigned value by test.
 * The class used in this file is defined in expr.h.
 */

//
//
//
//

/* This function move the pointer to skip space
 */
void skipSpace(const char ** strp) {
  while (isspace(**strp)) {
    *strp = *strp + 1;
  }
}

/* This function takes three parameter:
 * op (the pointer to the string of build-in functions)
 * lhs (the pointer to the Expreesion class)
 * rhs (the pointer to the Expression class)
 * and return the relative Expreesion class pointer based on op.
 */
Expression * makeExpr(char * op, Expression * lhs, Expression * rhs) {
  if (strcmp(op, "+") == 0) {
    free(op);
    return new PlusExpression(lhs, rhs);
  }
  else if (strcmp(op, "-") == 0) {
    free(op);
    return new MinusExpression(lhs, rhs);
  }
  else if (strcmp(op, "*") == 0) {
    free(op);
    return new TimesExpression(lhs, rhs);
  }
  else if (strcmp(op, "/") == 0) {
    free(op);
    return new DivExpression(lhs, rhs);
  }
  else if (strcmp(op, "%") == 0) {
    free(op);
    return new ModExpression(lhs, rhs);
  }
  else if (strcmp(op, "pow") == 0) {
    free(op);
    return new PowExpression(lhs, rhs);
  }
  else if (strcmp(op, "sin") == 0) {
    free(op);
    return new SinExpression(lhs);
  }
  else if (strcmp(op, "cos") == 0) {
    free(op);
    return new CosExpression(lhs);
  }
  else if (strcmp(op, "ln") == 0) {
    free(op);
    return new LogExpression(lhs);
  }
  else if (strcmp(op, "sqrt") == 0) {
    free(op);
    return new RootExpression(lhs);
  }
  else {
    std::cerr << "Impossible op char: " << op << "\n";
    free(op);
    exit(EXIT_FAILURE);
  }
}

/* This function get a string of char, which stop read when meet space or ')',
 * and return a pointer to this string
 */
char * getop(const char ** strp) {
  const char * temp = *strp;
  while ((!isspace(**strp)) && ((**strp) != ')')) {
    *strp = *strp + 1;
  }
  int n = (*strp) - temp;
  char * ans = strndup(temp, n);
  return ans;
}

/* This function is used to check if the ')' is showed at the end, if not, return NULL.
 */
Expression * endParse(const char ** strp, char * op, Expression * lhs, Expression * rhs) {
  skipSpace(strp);
  if (**strp == ')') {
    *strp = *strp + 1;
    return makeExpr(op, lhs, rhs);
  }
  std::cerr << "Expected ) but found " << *strp << "\n";
  free(op);
  delete lhs;
  delete rhs;
  return NULL;
}

/* This function is used to replace the parameter name with value in the rhs string saved before when we use a self-defined function to define a function.
 * After finish this replacement, we can parse it as a test instruction recursively
 */
void replace_vtod(std::string & str, vector * v) {
  //first, erase the space behind '(', to avoid take fname as parameter name
  while (str.find("( ") != std::string::npos) {
    size_t pos = str.find("( ");
    str.erase(pos + 1, 1);
  }
  //second, insert space before ')', to avoid confusing about ' x', ' xx' parameter like this
  size_t pos = 0;
  while (str.find(")", pos) != std::string::npos) {
    pos = str.find(")", pos);
    str.insert(pos, " ");
    pos = pos + 2;
  }
  //third, replace the ' (parameter name) ' with ' (parameter value) '
  for (size_t i = 0; i < (*v).size(); i++) {
    std::string var = (*v)[i].second;
    var = " " + var + " ";
    std::string num = std::to_string((*v)[i].first);
    num = " " + num + " ";
    while (str.find(var) != std::string::npos) {
      size_t des = str.find(var);
      str.insert(des, num);
      size_t len_var = var.size();
      size_t len_num = num.size();
      str.erase(des + len_num, len_var);
    }
  }
}

/* This function takes five parameter:
 * strp (the pointer to the pointer to a string that needs to be parsed)
 * str_hash (the std::hash that used to compute the hash value for a std::string)
 * hash_map_func (the map that used to save function name's hash value and its vector which used to save its parameter names and values) 
 * hash_map_rhs (the map that used to save function name's hash value and its rhs of the define instructions)
 * v (the pointer to the vector which consists of parameter names and values of the function)
 * and return the calculation result of the rhs of define instructions expression.
 */
double get_rhs_ans(const char ** strp,
                   std::hash<std::string> & str_hash,
                   std::unordered_map<size_t, vector> & hash_map_func,
                   std::unordered_map<size_t, std::string> & hash_map_rhs,
                   vector * v) {
  std::string right(*strp);
  size_t i = 0, pos = 0;
  //count the num of '()' to know if the input format is wrong, and find the last ')'
  do {
    if ((**strp) == '\n') {
      std::cerr << "Wrong Format of rhs of define.\n";
      exit(EXIT_FAILURE);
    }
    if ((**strp) == '(') {
      i++;
    }
    if ((**strp) == ')') {
      i--;
    }
    *strp = *strp + 1;
    pos++;
  } while (i != 0);
  //get the whole expression into a std::string
  std::string str = right.substr(0, pos + 1);
  replace_vtod(str, v);
  const char * temp = str.c_str();
  //get reuslt
  return parse_t(&temp, str_hash, hash_map_func, hash_map_rhs);
}

/* this function is used to determine if the function name is an one operand function
 */
bool is_one_op(char * op) {
  if ((strcmp(op, "sin") == 0) || (strcmp(op, "cos") == 0) || (strcmp(op, "ln") == 0) ||
      (strcmp(op, "sqrt") == 0)) {
    return true;
  }
  else {
    return false;
  }
}

/* this function is used to determine if the function name is a two operands function
 */
bool is_two_op(char * op) {
  if ((strcmp(op, "pow") == 0) || (strcmp(op, "+") == 0) || (strcmp(op, "-") == 0) ||
      (strcmp(op, "*") == 0) || (strcmp(op, "/") == 0) || (strcmp(op, "%") == 0)) {
    return true;
  }
  else {
    return false;
  }
}

/* This function takes six parameter:
 * strp (the pointer to the pointer to a string that needs to be parsed)
 * str_hash (the std::hash that used to compute the hash value for a std::string)
 * hash_map_func (the map that used to save function name's hash value and its vector which used to save its parameter names and values) 
 * hashNum (the hash value for the string of function name)
 * hash_map_rhs (the map that used to save function name's hash value and its rhs of the define instructions)
 * v (the pointer to the vector which consists of parameter names and values of the function)
 * and return the pointer to the Expression class that can be used to do evaluation to get result.
 */
Expression * parseOp(const char ** strp,
                     std::hash<std::string> & str_hash,
                     std::unordered_map<size_t, vector> & hash_map_func,
                     size_t hashNum,
                     std::unordered_map<size_t, std::string> & hash_map_rhs,
                     vector * v) {
  skipSpace(strp);
  //get fname
  char * op = getop(strp);
  if (is_one_op(op)) {
    Expression * lhs = parse(strp, str_hash, hash_map_func, hashNum, hash_map_rhs, v);
    if (lhs == NULL) {
      free(op);
      return NULL;
    }
    Expression * rhs = NULL;
    return endParse(strp, op, lhs, rhs);
  }
  else if (is_two_op(op)) {
    Expression * lhs = parse(strp, str_hash, hash_map_func, hashNum, hash_map_rhs, v);
    if (lhs == NULL) {
      free(op);
      return NULL;
    }
    Expression * rhs = parse(strp, str_hash, hash_map_func, hashNum, hash_map_rhs, v);
    if (rhs == NULL) {
      free(op);
      delete lhs;
      return NULL;
    }
    return endParse(strp, op, lhs, rhs);
  }
  else {
    // the function must be self-defined or undefined
    std::string fname(op);
    free(op);
    size_t hash_n = str_hash(fname);
    std::unordered_map<size_t, vector>::iterator it = hash_map_func.find(hash_n);
    if (it != hash_map_func.end()) {
      while ((**strp) != '(') {
        (*strp)--;
      }
      // turn the self-defined function expression into a double number
      double ans = get_rhs_ans(strp, str_hash, hash_map_func, hash_map_rhs, v);
      return new NumExpression(ans);
    }
    else {
      std::cerr << "Can not parse the function name\n";
      exit(EXIT_FAILURE);
    }
  }
}

/* This function takes six parameter:
 * strp (the pointer to the pointer to a string that needs to be parsed)
 * str_hash (the std::hash that used to compute the hash value for a std::string)
 * hash_map_func (the map that used to save function name's hash value and its vector which used to save its parameter names and values) 
 * hashNum (the hash value for the string of function name)
 * hash_map_rhs (the map that used to save function name's hash value and its rhs of the define instructions)
 * v (the pointer to the vector which consists of parameter names and values of the function)
 * and return the pointer to the Expression class that can be used to do evaluation to get result.
 */
Expression * parse(const char ** strp,
                   std::hash<std::string> & str_hash,
                   std::unordered_map<size_t, vector> & hash_map_func,
                   size_t hashNum,
                   std::unordered_map<size_t, std::string> & hash_map_rhs,
                   vector * v) {
  skipSpace(strp);
  if (**strp == '\0') {
    std::cerr << "End of line found mid expression!\n";
    return NULL;
  }
  else if (**strp == '(') {
    // (op E E)
    *strp = *strp + 1;
    return parseOp(strp, str_hash, hash_map_func, hashNum, hash_map_rhs, v);
  }
  else {
    //if it's a constant number
    char * name = getop(strp);
    char * endp;
    double num = strtod(name, &endp);
    if (check_digit(num, name) && ((*endp) == '\0') && ((*(--endp)) != '.')) {
      free(name);
      return new NumExpression(num);
    }
    // if it's a parameter name, find its value through vector
    std::string v_name(name);
    free(name);
    size_t i = 0;
    while ((*v)[i].second.compare(v_name) != 0) {
      i++;
      if ((*v).size() < i + 1) {
        std::cerr << "Parameter undefined\n";
        exit(EXIT_FAILURE);
      }
    }
    double ans = (*v)[i].first;
    return new NumExpression(ans);
  }
}
