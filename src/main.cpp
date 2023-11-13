#include <cstdlib>
#include <iostream>
#include <fstream>
#include "../inc/Json.hpp"

int main(void)
{
  json::Object jobj;

  jobj.insert("a", json::Value(json::Raw("raw")));
  jobj.insert("b", json::Value("rawr"));

  return EXIT_SUCCESS;
}