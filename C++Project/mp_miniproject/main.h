#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
class Expression
{
 public:
  virtual std::string toString() const = 0;
  virtual double evaluate() const = 0;
  virtual ~Expression() {}
};
class NumExpression : public Expression
{
  double num;

 public:
  NumExpression(double n) : num(n) {}
  virtual std::string toString() const {
    std::string answer;
    std::stringstream stream;
    stream << num;
    answer = stream.str();
    stream.clear();
    return answer;
  }
  virtual double evaluate() const { return num; }
  virtual ~NumExpression() {}
};

class PlusExpression : public Expression
{
  Expression * _lhs;
  Expression * _rhs;

 public:
  PlusExpression(Expression * lhs, Expression * rhs) : _lhs(lhs), _rhs(rhs) {}
  virtual std::string toString() const {
    std::string answer;
    std::stringstream stream;
    stream << "(" << _lhs->toString() << " + " << _rhs->toString() << ")";
    answer = stream.str();
    stream.clear();
    return answer;
  }
  virtual double evaluate() const {
    double answer = 0;
    answer = _lhs->evaluate();
    answer += _rhs->evaluate();
    return answer;
  }

  virtual ~PlusExpression() {
    delete _lhs;
    delete _rhs;
  }
};

class MinusExpression : public Expression
{
  Expression * _lhs;
  Expression * _rhs;

 public:
  MinusExpression(Expression * lhs, Expression * rhs) : _lhs(lhs), _rhs(rhs) {}
  virtual std::string toString() const {
    std::string answer;
    std::stringstream stream;
    stream << "(" << _lhs->toString() << " - " << _rhs->toString() << ")";
    answer = stream.str();
    stream.clear();
    return answer;
  }
  virtual double evaluate() const {
    double answer = 0;
    answer = _lhs->evaluate();
    answer -= _rhs->evaluate();
    return answer;
  }
  virtual ~MinusExpression() {
    delete _lhs;
    delete _rhs;
  }
};

class TimesExpression : public Expression
{
  Expression * _lhs;
  Expression * _rhs;

 public:
  TimesExpression(Expression * lhs, Expression * rhs) : _lhs(lhs), _rhs(rhs) {}
  virtual std::string toString() const {
    std::string answer;
    std::stringstream stream;
    stream << "(" << _lhs->toString() << " * " << _rhs->toString() << ")";
    answer = stream.str();
    stream.clear();
    return answer;
  }
  virtual double evaluate() const {
    double answer = 0;
    answer = _lhs->evaluate();
    answer *= _rhs->evaluate();
    return answer;
  }

  virtual ~TimesExpression() {
    delete _lhs;
    delete _rhs;
  }
};

class DivExpression : public Expression
{
  Expression * _lhs;
  Expression * _rhs;

 public:
  DivExpression(Expression * lhs, Expression * rhs) : _lhs(lhs), _rhs(rhs) {}
  virtual std::string toString() const {
    std::string answer;
    std::stringstream stream;
    stream << "(" << _lhs->toString() << " / " << _rhs->toString() << ")";
    answer = stream.str();
    stream.clear();
    return answer;
  }
  virtual double evaluate() const {
    double answer = 0;
    answer = _lhs->evaluate();
    answer /= _rhs->evaluate();
    return answer;
  }

  virtual ~DivExpression() {
    delete _lhs;
    delete _rhs;
  }
};

class ModExpression : public Expression
{
  Expression * _lhs;
  Expression * _rhs;

 public:
  ModExpression(Expression * lhs, Expression * rhs) : _lhs(lhs), _rhs(rhs) {}
  virtual std::string toString() const {
    std::string answer;
    std::stringstream stream;
    stream << "(" << _lhs->toString() << " % " << _rhs->toString() << ")";
    answer = stream.str();
    stream.clear();
    return answer;
  }
  virtual double evaluate() const {
    double answer = 0;
    answer = fmod(_lhs->evaluate(), _rhs->evaluate());
    return answer;
  }

  virtual ~ModExpression() {
    delete _lhs;
    delete _rhs;
  }
};

class PowExpression : public Expression
{
  Expression * _lhs;
  Expression * _rhs;

 public:
  PowExpression(Expression * lhs, Expression * rhs) : _lhs(lhs), _rhs(rhs) {}
  virtual std::string toString() const {
    std::string answer;
    std::stringstream stream;
    stream << "(" << _lhs->toString() << " ^ " << _rhs->toString() << ")";
    answer = stream.str();
    stream.clear();
    return answer;
  }
  virtual double evaluate() const {
    double answer = 0;
    answer = pow(_lhs->evaluate(), _rhs->evaluate());
    return answer;
  }

  virtual ~PowExpression() {
    delete _lhs;
    delete _rhs;
  }
};

class SinExpression : public Expression
{
  Expression * _lhs;

 public:
  SinExpression(Expression * lhs) : _lhs(lhs) {}
  virtual std::string toString() const {
    std::string answer;
    std::stringstream stream;
    stream << "(sin( " << _lhs->toString() << " ))";
    answer = stream.str();
    stream.clear();
    return answer;
  }
  virtual double evaluate() const {
    double answer = 0;
    answer = sin(_lhs->evaluate());
    return answer;
  }

  virtual ~SinExpression() { delete _lhs; }
};

class CosExpression : public Expression
{
  Expression * _lhs;

 public:
  CosExpression(Expression * lhs) : _lhs(lhs) {}
  virtual std::string toString() const {
    std::string answer;
    std::stringstream stream;
    stream << "(cos( " << _lhs->toString() << " ))";
    answer = stream.str();
    stream.clear();
    return answer;
  }
  virtual double evaluate() const {
    double answer = 0;
    answer = cos(_lhs->evaluate());
    return answer;
  }

  virtual ~CosExpression() { delete _lhs; }
};

class LogExpression : public Expression
{
  Expression * _lhs;

 public:
  LogExpression(Expression * lhs) : _lhs(lhs) {}
  virtual std::string toString() const {
    std::string answer;
    std::stringstream stream;
    stream << "(log( " << _lhs->toString() << " ))";
    answer = stream.str();
    stream.clear();
    return answer;
  }
  virtual double evaluate() const {
    double answer = 0;
    answer = log(_lhs->evaluate());
    return answer;
  }

  virtual ~LogExpression() { delete _lhs; }
};

class RootExpression : public Expression
{
  Expression * _lhs;

 public:
  RootExpression(Expression * lhs) : _lhs(lhs) {}
  virtual std::string toString() const {
    std::string answer;
    std::stringstream stream;
    stream << "(sqrt( " << _lhs->toString() << " ))";
    answer = stream.str();
    stream.clear();
    return answer;
  }
  virtual double evaluate() const {
    double answer = 0;
    answer = sqrt(_lhs->evaluate());
    return answer;
  }

  virtual ~RootExpression() { delete _lhs; }
};

typedef std::vector<std::pair<double, std::string> > vector;
void skipSpace(const char ** strp);
char * getop(const char ** strp);
size_t parse_lhs(const char ** strp,
                 std::hash<std::string> & str_hash,
                 std::unordered_map<size_t, vector> & hash_map_func);
std::string save_rhs(const char ** strp);
double get_result(const char ** strp,
                  std::hash<std::string> & str_hash,
                  std::unordered_map<size_t, vector> & hash_map_func,
                  size_t hashNum,
                  std::unordered_map<size_t, std::string> & hash_map_rhs,
                  vector * v);
double parse_t(const char ** strp,
               std::hash<std::string> & str_hash,
               std::unordered_map<size_t, vector> & hash_map_func,
               std::unordered_map<size_t, std::string> & hash_map_rhs);
bool test_equal(double x, double y);
std::pair<std::string, double> * parse_numint(const char ** strp,
                                              std::vector<std::pair<double, double> > & v_range,
                                              std::vector<double> & v_var);
double integral(std::string fname,
                size_t size,
                size_t n,
                double step,
                std::vector<std::pair<double, double> > & v_range,
                std::vector<double> & v_var,
                std::hash<std::string> & str_hash,
                std::unordered_map<size_t, vector> & hash_map_func,
                std::unordered_map<size_t, std::string> & hash_map_rhs);
double monte_carlo(std::string fname,
                   size_t size,
                   size_t num,
                   std::vector<std::pair<double, double> > & v_range,
                   std::hash<std::string> & str_hash,
                   std::unordered_map<size_t, vector> & hash_map_func,
                   std::unordered_map<size_t, std::string> & hash_map_rhs);
bool check_step(double step, std::vector<std::pair<double, double> > & v_range);
bool check_num(int num, double step);
double * getNum(const char ** strp);
std::vector<double> gradient_method(int cont,
                                    std::string fname,
                                    double gamma,
                                    double convergedDistance,
                                    std::vector<double> & P_curr,
                                    int num,
                                    std::hash<std::string> & str_hash,
                                    std::unordered_map<size_t, vector> & hash_map_func,
                                    std::unordered_map<size_t, std::string> & hash_map_rhs);
void printPoint(std::vector<double> & ans);
void define(const char * temp,
            const char * mid,
            std::string lhs,
            std::hash<std::string> & str_hash,
            std::unordered_map<size_t, vector> & hash_map_func,
            std::unordered_map<size_t, std::string> & hash_map_rhs);
void test(const char * temp,
          const char * mid,
          std::string lhs,
          std::hash<std::string> & str_hash,
          std::unordered_map<size_t, vector> & hash_map_func,
          std::unordered_map<size_t, std::string> & hash_map_rhs);
void trapezoid(const char * temp,
               std::hash<std::string> & str_hash,
               std::unordered_map<size_t, vector> & hash_map_func,
               std::unordered_map<size_t, std::string> & hash_map_rhs);
void random_monteCarlo(const char * temp,
                       std::hash<std::string> & str_hash,
                       std::unordered_map<size_t, vector> & hash_map_func,
                       std::unordered_map<size_t, std::string> & hash_map_rhs);
void get_limit(const char * temp,
               int cont,
               std::hash<std::string> & str_hash,
               std::unordered_map<size_t, vector> & hash_map_func,
               std::unordered_map<size_t, std::string> & hash_map_rhs);
