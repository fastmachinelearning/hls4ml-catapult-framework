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

#include <string.h>
#include "mc_scverify.h"

#include "firmware/myproject.h"
#include "firmware/nnet_utils/nnet_helpers.h"

//hls-fpga-machine-learning weights headfiles

#define CHECKPOINT 5000

namespace nnet {
    bool trace_enabled = true;
    std::map<std::string, void *> *trace_outputs = NULL;
    size_t trace_type_size = sizeof(double);
}

template<class T> 
void print_fxd_as_bin(std::ostream &out, T data) {
    for (int j = data.length() - 1; j >= 0; j--) {
        out << data[j];
    }
}

CCS_MAIN(int argc, char **argv)
{
  //load input data from text file
  std::ifstream fin("tb_data/tb_input_features.dat");
  //load predictions from text file
  std::ifstream fpr("tb_data/tb_output_predictions.dat");
  
  //hls-fpga-machine-learning insert load weights

	std::cout << "Mentor Graphics Catapult HLS" << std::endl;
#ifdef RTL_SIM
  std::string RESULTS_LOG = "tb_data/catapult_rtl_cosim_results.log";
#else
  std::string RESULTS_LOG = "tb_data/catapult_csim_results.log";
#endif
  std::ofstream fout(RESULTS_LOG);

	std::string INPUT_FILE_BIN_MEM = "tb_data/tb_input_features.mem";
  std::string OUTPUT_FILE_BIN_MEM = "tb_data/tb_output_predictions.mem";
  std::ofstream fout_ifbm(INPUT_FILE_BIN_MEM);
  std::ofstream fout_ofbm(OUTPUT_FILE_BIN_MEM);

  //save traces step1

  std::string iline;
  std::string pline;
  int e = 0;

  if (fin.is_open() && fpr.is_open()) {
    while ( std::getline(fin,iline) && std::getline (fpr,pline) ) {
      if (e % CHECKPOINT == 0) std::cout << "Processing input " << e << std::endl;
      e++;
      char* cstr=const_cast<char*>(iline.c_str());
      char* current;
      std::vector<float> in;
      current=strtok(cstr," ");
      while(current!=NULL) {
        in.push_back(atof(current));
        current=strtok(NULL," ");
      }
      cstr=const_cast<char*>(pline.c_str());
      std::vector<float> pr;
      current=strtok(cstr," ");
      while(current!=NULL) {
        pr.push_back(atof(current));
        current=strtok(NULL," ");
      }

      //hls-fpga-machine-learning insert data

      //hls-fpga-machine-learning insert top-level-function

      //hls-fpga-machine-learning insert tb-output

      if (e % CHECKPOINT == 0) {
        std::cout << "Predictions" << std::endl;
        //hls-fpga-machine-learning insert predictions
        std::cout << "Quantized predictions" << std::endl;
        //hls-fpga-machine-learning insert quantized
      }
      
      //save traces step2
    }
    fin.close();
    fpr.close();
  } else {
    std::cout << "INFO: Unable to open input/predictions file, using default input." << std::endl;
    //hls-fpga-machine-learning insert zero

    //hls-fpga-machine-learning insert top-level-function

    //hls-fpga-machine-learning insert output

    //save traces step2

    //hls-fpga-machine-learning insert tb-output
  }

  fout.close();
  std::cout << "INFO: Saved inference results to file: " << RESULTS_LOG << std::endl;

	fout_ifbm.close();
  std::cout << "INFO: Saved input data to .mem file: " << INPUT_FILE_BIN_MEM << std::endl;
  fout_ofbm.close();
  std::cout << "INFO: Saved output data to .mem file: " << OUTPUT_FILE_BIN_MEM << std::endl;
  
  //save traces step3

  CCS_RETURN(0);
}
