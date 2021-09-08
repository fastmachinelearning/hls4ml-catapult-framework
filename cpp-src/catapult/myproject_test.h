#ifndef _MYPROJECT_TEST_H
#define _MYPROJECT_TEST_H

#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mc_scverify.h>
#include <string.h>

#include "myproject.h"
#include "nnet_utils/nnet_helpers.h"

inline void split(std::string input, std::vector<std::string> &result,
                  const char *delimiter)
{
  char* cstr = const_cast<char*>(input.c_str());
  char* current;
  current= strtok(cstr, delimiter);
  while (current != NULL) {
    result.push_back(std::string(current));
    current = strtok(NULL, delimiter);
  }
}

inline int check(const int n_input, const int n_output)
{
  //load input data from text file
  std::ifstream fin("tb_data/tb_input_features.dat");
  //load predictions from text file
  std::ifstream fpr("tb_data/tb_output_predictions.dat");

  bool pr_present = fpr.is_open();

  std::string iline;
  std::string pline;

  if (fin.is_open()) {
    double max_relative_diff = 0.0;

    while ( std::getline(fin,iline)) {
      if (pr_present) {
        std::getline(fpr,pline);
      }

      // Read test data
      std::vector<std::string> in_nums;
      split(iline, in_nums, ";");

      test_input_t input[n_input];

      for (int i = 0; i < n_input; i++) {
        std::vector<std::string> int_strs;
        split(in_nums[i], int_strs, " ");

        const int size =
          test_input_t::width / 32 + (test_input_t::width % 32 > 0);

        int ints[size];
        for (int i = 0; i < size; i++) {
          ints[i] = atoi(int_strs[i].c_str());
        }

        input[i].bit_fill<size>(ints);
      }

      test_output_t pr[n_output];

      if (pr_present) {
        std::vector<std::string> pr_nums;
        split(pline, pr_nums, ";");

        for (int i = 0; i < n_output; i++) {
          std::vector<std::string> int_strs;
          split(pr_nums[i], int_strs, " ");

          const int size =
            test_output_t::width / 32 + (test_output_t::width % 32 > 0);
          int ints[size];
          for (int i = 0; i < size; i++) {
            ints[i] = atoi(int_strs[i].c_str());
          }

          pr[i].bit_fill<size>(ints);
        }
      }

      // Run the code
      test_output_t output[n_output];

      unsigned short size_in1,size_out1;
      CCS_DESIGN(myproject) (input,output,size_in1,size_out1);

      int different = 0;

      // Evaluate the results
      std::cout << "Inputs:" << std::endl;
      for (int i = 0; i < n_input; i++) {
        std::cout << input[i] << " ";
      }
      std::cout << std::endl;

      if (pr_present) {
        std::cout << "Expected:" << std::endl;
        for (int i = 0; i < n_output; i++) {
          std::cout << pr[i] << " ";
        }
        std::cout << std::endl;
      }

      double absolute_diff[n_output];
      double relative_diff[n_output];
      
      double max_relative_diff_local = 0.0;
      bool compared_against_zero = false;

      std::cout << "Results:" << std::endl;
      for (int i = 0; i < n_output; i++) {
        std::cout << output[i] << " ";
        if (pr_present) {
          absolute_diff[i] = std::abs(output[i].to_double() - pr[i].to_double());

          if (output[i] == pr[i]) {
            relative_diff[i] = 0.0;
          } else if (output[i] == 0 || pr[i] == 0) {
            relative_diff[i] = -1.0;
            compared_against_zero = true;
          } else {
            relative_diff[i] = absolute_diff[i] / std::abs(pr[i].to_double());
          }

          max_relative_diff_local = std::max(max_relative_diff_local,
                                             relative_diff[i]);
        }
      }
      std::cout << std::endl;

      max_relative_diff = std::max(max_relative_diff, max_relative_diff_local);

      std::cout << "{{LARGEST_RELATIVE_DIFFERENCE=" << max_relative_diff_local;
      std::cout << "}}" << std::endl;

      if (compared_against_zero) {
        std::cout << "{{COMPARISON_AGAINST_ZERO_DETECTED}}" << std::endl;
      }

      std::cout << "===" << std::endl;
    }

    std::cout << "{{LARGEST_RELATIVE_DIFFERENCE_OVERALL=" << max_relative_diff;
    std::cout << "}}" << std::endl;

    fin.close();
    fpr.close();
    
    return 0;
  } else {
    std::cout << "INFO: Unable to open input file." << std::endl;
    return 1;
  }
}

#endif
