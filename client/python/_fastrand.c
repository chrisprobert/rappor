/*
Copyright 2014 Google Inc. All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/*
 * _fastrand.c -- Python extension module to generate random bit vectors
 * quickly.
 *
 * IMPORTANT: This module does not use crytographically strong randomness.  It
 * should be used ONLY be used to speed up the simulation.  Don't use it in
 * production.
 *
 * If an adversary can predict which random bits are flipped, then RAPPOR's
 * privacy is compromised.
 *
 */

#include <stdint.h>  // uint64_t
#include <stdio.h>  // printf
#include <stdlib.h>  // srand
#include <time.h>  // time

#include <Python.h>

uint64_t randbits(float p1, int num_bits) {
  uint64_t result = 0;
  int threshold = (int)(p1 * RAND_MAX);
  int i;
  for (i = 0; i < num_bits; ++i) {
    uint64_t bit = (rand() < threshold);
    result |= (bit << i);
  }
  return result;
}

static PyObject *
func_randbits(PyObject *self, PyObject *args) {
  float p1;
  int num_bits;

  if (!PyArg_ParseTuple(args, "fi", &p1, &num_bits)) {
    return NULL;
  }
  if (p1 < 0.0 || p1 > 1.0) {
    printf("p1 must be between 0.0 and 1.0\n");
    // return None for now; easier than raising ValueError
    Py_INCREF(Py_None);
    return Py_None;
  }
  if (num_bits < 0 || num_bits > 64) {
    printf("num_bits must be 64 or less\n");
    // return None for now; easier than raising ValueError
    Py_INCREF(Py_None);
    return Py_None;
  }

  //printf("p: %f\n", p);
  uint64_t r = randbits(p1, num_bits);
  return PyLong_FromUnsignedLongLong(r);
}

PyMethodDef methods[] = {
  {"randbits", func_randbits, METH_VARARGS,
   "Return a number with N bits, where each bit is 1 with probability p."},
  {NULL, NULL},
};

void init_fastrand(void) {
  Py_InitModule("_fastrand", methods);

  // Just seed it here; we don't give the application any control.
  int seed = time(NULL);
  srand(seed);
}
