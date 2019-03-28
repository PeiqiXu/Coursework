#include "main.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <cstdlib>
#include <iostream>
#include <random>
#include <unordered_map>
#include <utility>

/* This program can fulfill six functions:
 * input 'define' to define a new function expression,
 * input 'test' to test two results between expressions or constant numbers,
 * input 'numint' to using trapezoid method to calculate integrals of a function,
 * input 'mcint' to using montecarlo method to calculate integrals of a function,
 * input 'max' to using gradient method to calculate the maxmium of a function,
 * input 'min' to using gradient method to calculate the minimum of a function.
 * specifically, you can input 'exit' to exit the program at any time.
 */

//
//
//
//

/* This function is used to check if the end of the instruction have any trash words.
 * Legally, you can use # to input comments without interfering the parsing process.
 */
bool check_end(const char ** strp) {
  skipSpace(strp);
  if ((**strp) == '\0') {
    return true;
  }
  else if ((**strp) == '#') {
    return true;
  }
  else {
    return false;
  }
}

/* This function is used to parse the instructions.
 */
int parse_ins(const char ** strp) {
  skipSpace(strp);
  if ((**strp) == '#') {
    return 0;
  }
  char * op = getop(strp);
  if (strcmp(op, "define") == 0) {
    free(op);
    return 1;
  }
  else if (strcmp(op, "test") == 0) {
    free(op);
    return 2;
  }
  else if (strcmp(op, "numint") == 0) {
    free(op);
    return 3;
  }
  else if (strcmp(op, "mcint") == 0) {
    free(op);
    return 4;
  }
  else if (strcmp(op, "max") == 0) {
    free(op);
    return 5;
  }
  else if (strcmp(op, "min") == 0) {
    free(op);
    return 6;
  }
  else if (strcmp(op, "exit") == 0) {
    free(op);
    return 7;
  }
  else {
    std::cerr << "Expected a legal instruction, but found " << op << "\n";
    free(op);
    exit(EXIT_FAILURE);
  }
}

/* This function is used to save the build-in functions into maps at first.
 */
void prepare(std::hash<std::string> & str_hash,
             std::unordered_map<size_t, vector> & hash_map_func,
             std::unordered_map<size_t, std::string> & hash_map_rhs) {
  char ** package = new char *[10];
  package[0] = strdup("define (+ x y)=(+ x y)");
  package[1] = strdup("define (- x y)=(- x y)");
  package[2] = strdup("define (* x y)=(* x y)");
  package[3] = strdup("define (/ x y)=(/ x y)");
  package[4] = strdup("define (% x y)=(% x y)");
  package[5] = strdup("define (pow x y)=(pow x y)");
  package[6] = strdup("define (ln x)=(ln x)");
  package[7] = strdup("define (sin x)=(sin x)");
  package[8] = strdup("define (cos x)=(cos x)");
  package[9] = strdup("define (sqrt x)=(sqrt x)");
  for (int i = 0; i < 10; i++) {
    const char * temp = package[i];
    int cont = parse_ins(&temp);
    if (cont == 1) {
      size_t hashNum = parse_lhs(&temp, str_hash, hash_map_func);
      temp += 1;
      std::string rhs = save_rhs(&temp);
      std::pair<size_t, std::string> rhs_string(hashNum, rhs);
      hash_map_rhs.insert(rhs_string);
    }
    free(package[i]);
  }
  delete[](package);
}

/* This function is used to rephrase the input expression to a printable order.
 */
void rephrase(std::string & str) {
  //remove blank space after '('
  while (str.find("( ") != std::string::npos) {
    size_t pos = str.find("( ");
    str.erase(pos + 1, 1);
  }

  size_t pos = 0;
  //insert blank space between ')'
  while (str.find(")", pos) != std::string::npos) {
    pos = str.find(")", pos);
    str.insert(pos + 1, " ");
    pos++;
  }
  pos = 0;
  //change the order of '(' and function name
  while (str.find("(", pos) != std::string::npos) {
    pos = str.find("(", pos);
    str.erase(pos, 1);
    pos = str.find(" ", pos);
    str.insert(pos, "(");
    str.erase(pos + 1, 1);
    pos++;
  }
  //remove blank space after'('
  while (str.find("( ") != std::string::npos) {
    size_t pos = str.find("( ");
    str.erase(pos + 1, 1);
  }
  //replace double blank space with one blank space
  while (str.find("  ") != std::string::npos) {
    size_t pos = str.find("  ");
    str.erase(pos + 1, 1);
  }
  //remove blank space before ')'
  while (str.find(" )") != std::string::npos) {
    size_t pos = str.find(" )");
    str.erase(pos, 1);
  }
  //remove the '\n'
  if (str.find('\n') != std::string::npos) {
    size_t pos = str.find('\n');
    str.erase(pos, 1);
  }
}

int main(void) {
  std::hash<std::string> str_hash;
  std::unordered_map<size_t, vector> hash_map_func;
  std::unordered_map<size_t, std::string> hash_map_rhs;
  prepare(str_hash, hash_map_func, hash_map_rhs);
  char * line = NULL;
  size_t sz;
  while (getline(&line, &sz, stdin) != -1) {
    const char * temp = line;
    int cont = parse_ins(&temp);
    skipSpace(&temp);
    const char * mid = temp;
    std::string lhs(mid);
    if (cont == 1) {
      define(temp, mid, lhs, str_hash, hash_map_func, hash_map_rhs);
    }
    if (cont == 2) {
      test(temp, mid, lhs, str_hash, hash_map_func, hash_map_rhs);
    }
    if (cont == 3) {
      trapezoid(temp, str_hash, hash_map_func, hash_map_rhs);
    }
    if (cont == 4) {
      random_monteCarlo(temp, str_hash, hash_map_func, hash_map_rhs);
    }
    if (cont == 5 || cont == 6) {
      get_limit(temp, cont, str_hash, hash_map_func, hash_map_rhs);
    }
    if (cont == 7) {
      break;
    }
  }
  free(line);
  return EXIT_SUCCESS;
}
