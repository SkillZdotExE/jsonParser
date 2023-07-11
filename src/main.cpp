#include <cstdlib>
#include <iostream>
#include <fstream>
#include "../inc/Json.hpp"

int main(void)
{
  json::JsonObject jobj;
  jobj.readFromFile("data/json1.json");
  json::JsonFormattingOptions options;
  std::cout << jobj.getStringF() << "\n";

  return EXIT_SUCCESS;
}