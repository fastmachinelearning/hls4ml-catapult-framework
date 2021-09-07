//
//    rfnoc-hls-neuralnet: Vivado HLS code for neural-net building blocks
//
//    Copyright (C) 2017 EJ Kreinar
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "firmware/myproject.h"
#include "firmware/nnet_utils/nnet_helpers.h"

#define N_INPUT {{N_INPUT}}
#define N_OUTPUT {{N_OUTPUT}}

typedef {{INPUT_TYPE}} test_input_t;
typedef {{OUTPUT_TYPE}} test_output_t;

namespace nnet {
  bool trace_enabled = true;
  std::map<std::string, void *> *trace_outputs = NULL;
  size_t trace_type_size = sizeof(double);
}

inline void split(std::string input, std::vector<std::string> &result,
                  const char *delimiter)
{
  char* cstr=const_cast<char*>(input.c_str());
  char* current;
  current=strtok(cstr,delimiter);
  while(current!=NULL) {
    result.push_back(std::string(current));
    current=strtok(NULL,delimiter);
  }
}

template<class T>
inline void convert(std::vector<int> ints, T &result)
{
  int N = T::width;
  int bits = N;
  int i = ints.size() - 1;

  while (bits > 0) {
    result(std::min(N - bits + 31, N - 1), N - bits) = ints[i];
    bits -= 32;
    i--;
  }
}

template<class T>
inline void convert(T input, std::vector<int> &result)
{
  int N = T::width;
  int bits = N;
  result.clear();

  while (bits > 0) {
    result.insert(result.begin(), input(std::min(N - bits + 31, N - 1),
                                        N - bits));
    bits -= 32;
  }
}

inline void save(std::vector<std::vector<int> > &outputs, std::ofstream &fout)
{
  std::string sep_outer = "";
  for (int i = 0; i < outputs.size(); i++) {
    std::string sep_inner = "";

    fout << sep_outer;
    sep_outer = ";";

    for (int j = 0; j < outputs[i].size(); j++) {
      fout << sep_inner << outputs[i][j];
      sep_inner = " ";
    }
  }

  fout << std::endl;
}

int main(int argc, char **argv)
{
  //load input data from text file
  std::ifstream fin("tb_data/tb_input_features.dat");
  //load predictions from text file
  std::ifstream fpr("tb_data/tb_output_predictions.dat");

  bool pr_present = fpr.is_open();
  std::string RESULTS_LOG;

  if (pr_present) {
#ifdef RTL_SIM
    RESULTS_LOG = "tb_data/rtl_cosim_results.log";
#else
    RESULTS_LOG = "tb_data/csim_results.log";
#endif
  } else {
    RESULTS_LOG = "tb_data/tb_output_predictions.dat";
  }
  std::ofstream fout(RESULTS_LOG);

  std::string iline;
  std::string pline;

  if (fin.is_open()) {
    while ( std::getline(fin,iline)) {
      if (pr_present) {
        std::getline(fpr,pline);
      }

      // Read test data
      std::vector<std::string> in_nums;
      split(iline, in_nums, ";");

      test_input_t model_in[N_INPUT];

      for (int i = 0; i < N_INPUT; i++) {
        std::vector<std::string> int_strs;
        split(in_nums[i], int_strs, " ");

        std::vector<int> ints;
        for (int i = 0; i < int_strs.size(); i++) {
          ints.push_back(atoi(int_strs[i].c_str()));
        }

        convert<test_input_t>(ints, model_in[i]);
      }

      test_output_t pr[N_OUTPUT];

      if (pr_present) {
        std::vector<std::string> pr_nums;
        split(pline, pr_nums, ";");

        for (int i = 0; i < N_OUTPUT; i++) {
          std::vector<std::string> int_strs;
          split(pr_nums[i], int_strs, " ");

          std::vector<int> ints;
          for (int i = 0; i < int_strs.size(); i++) {
            ints.push_back(atoi(int_strs[i].c_str()));
          }

          convert<test_output_t>(ints, pr[i]);
        }
      }

      // Run the code
      test_output_t model_out[N_OUTPUT];

      unsigned short size_in1,size_out1;
      myproject(model_in,model_out,size_in1,size_out1);

      int different = 0;

      // Evaluate the results
      std::cout << "Inputs:" << std::endl;
      for (int i = 0; i < N_INPUT; i++) {
        std::cout << model_in[i] << " ";
      }
      std::cout << std::endl;

      if (pr_present) {
        std::cout << "Expected:" << std::endl;
        for (int i = 0; i < N_OUTPUT; i++) {
          std::cout << pr[i] << " ";
        }
        std::cout << std::endl;
      }

      std::cout << "Results:" << std::endl;
      for (int i = 0; i < N_OUTPUT; i++) {
        std::cout << model_out[i] << " ";

        if (pr_present && model_out[i] != pr[i]) {
          different += 1;
        }
      }
      std::cout << std::endl;

      std::cout << "Differences: " << different << std::endl;

      std::cout << "===" << std::endl;

      std::vector<std::vector<int> > outputs;
      for (int i = 0; i < N_OUTPUT; i++) {
        std::vector<int> output;
        convert<test_output_t>(model_out[i], output);
        outputs.push_back(output);
      }

      save(outputs, fout);
    }
    fin.close();
    fpr.close();
  } else {
    std::cout << "INFO: Unable to open input file." << std::endl;
  }

  fout.close();
  std::cout << "INFO: Saved inference results to file: " << RESULTS_LOG
            << std::endl;

  return 0;
}
