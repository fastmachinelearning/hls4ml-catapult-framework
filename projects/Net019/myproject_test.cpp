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
#include "myproject.h"

typedef input_t test_input_t;
typedef result_t test_output_t;

#include "myproject_test.h"

#define N_INPUT N_INPUT_1_1
#define N_OUTPUT N_LAYER_4

namespace nnet {
  bool trace_enabled = true;
  std::map<std::string, void *> *trace_outputs = NULL;
  size_t trace_type_size = sizeof(double);
}

CCS_MAIN(int argc, char **argv)
{
  CCS_RETURN(check(N_INPUT, N_OUTPUT));
}
