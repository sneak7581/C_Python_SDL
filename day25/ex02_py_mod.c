#include <Python.h>
#include <stdio.h>

static PyObject *hello()
{
  puts("hello py module in c");
  return Py_BuildValue("");
}

static PyObject *c_add(PyObject *self,PyObject *args)
{
  int a,b,r;
  if(!PyArg_ParseTuple(args,"ii",&a,&b)) //파이썬에서 받은 데이터 언패킹 
    return NULL;
  
  r = a+b;
  return Py_BuildValue("i",r); //패킹해서 파이썬쪽으로 넘겨주기 
}

static struct PyMethodDef _sample_methods[] =
{
  {"hello",hello,METH_NOARGS,NULL},
  {"c_add",c_add,METH_VARARGS,NULL},
  {NULL}  
};

static struct PyModuleDef _sample_module = {
  PyModuleDef_HEAD_INIT,
  "_ex02",
  "this is sample C module",
  -1,
  _sample_methods
};

PyMODINIT_FUNC PyInit__ex02(void)
{
  PyObject *m;
  m = PyModule_Create(&_sample_module);
  return m;
}




// #include <Python.h>
// #include <stdio.h>

// static PyObject *hello()
// {
//     puts("hello py modules in C\n");
//     return Py_BuildValue("");
// }

// static PyObject *c_add(PyObject *self,PyObject *args)
// {
//   int a,b,r;
//   if(!PyArg_ParseTuple(args,"ii",&a,&b)) //파이썬에서 받은 데이터 언패킹 
//     return NULL;
  
//   r = a+b;
//   return Py_BuildValue("i",r); //패킹해서 파이썬쪽으로 넘겨주기 
// }

// static struct PyMethodDef _sample_methods[] =
// {
//     {"hello", hello,METH_NOARGS, NULL},   // "hello" 문자열을 받으면 , hello 함수를 호출하라 (함수포인터) argument 가 없는 함수
//     {"c_add",c_add,METH_VARARGS,NULL},
//     {NULL} // 끝을 표기해줌 
// };

// static struct PyModuleDef _sample_module =
// {
//     PyModuleDef_HEAD_INIT, // 모듈 헤드를 지정해줌 (함수를 모듈로 묶는 것) 현재 함수가 하나라 하나만 묶으면 됌 ( 모듈 하나, 함수하나 ) 보통 모듈 안에 함수 여러개가 온다
//     "_ex02", // 모듈 이름 정의해줌
//     "this is sample C module", // 모듈에 대한 설명
//     -1,
//     _sample_methods // 메소드 정의
// };

// // 모듈이 초기화 되었을때 호출되는 함수 정의
// PyMODINIT_FUNC PyInit__ex02(void) // PyInit_(모듈이름) 현재 모듈 이름이 _ex02 이다 맞춰주는게 코드보기 좋다
// {
//     PyObject *m;
//     m = PyModule_Create(&_sample_module);
//     return m;
// }
