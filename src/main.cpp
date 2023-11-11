#include <cstdlib>
#include <iostream>
#include <fstream>
#include "../inc/Json.hpp"

int main(void)
{
  json::JsonObject jobj;
  
  jobj.insert("a", json::JsonValue(json::Raw("raw")));
  jobj.insert("b", json::JsonValue("rawr"));


  return EXIT_SUCCESS;
}