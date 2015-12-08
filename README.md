# S3FP
----



## About S3FP
----------
S3FP is a dynamic testing tool for triggering high floating-point error scenarios of programs. 
Currently, the high floating-point error scenarios include: 

- high round-off error occuring on a variable

- divergence (discrete value deviation) 

The error scenarios are triggered with witnessing inputs. 
Such inputs can be useful in various purposes such as program analysis and tuning. 

For more information, please refer to the related publications: 

- Wei-Fan Chiang, Ganesh Gopalakrishnan, Zvonimir Rakamaric, and Alexey Solovyev. "Efficient search for inputs causing high floating-point errors". In PPoPP, 2014. 



## Installation
------------
Please just use the python script "s3fp-install.py" for installation by 

```
s3fp-install.py build
```


## Usage
-----
### Manual Generation of the Real-number Versions 
S3FP performs error estimations based on the comparisons between the original programs and their corresponding real-number versions. 
The real-number versions can be simulated by using very high bit-width floating-point numbers such as 128-bit numbers. 
At this point, the users need to manually generate the real-number versions of their programs. 


### Manual Program Instrumentation 
S3FP currently requires users to manually instrument their programs in order to 

- accept the binary-form inputs enumerated by S3FP and 

- export the binary-form outputs for the search performed by S3FP. 

#### Input 
An input generated by S3FP is a (binary) stream of 32-bit floating-point numbers. 

#### Output
A program output to S3FP should be a stream of 128-bit floating-point numbers (__float128 type in C). 
The stream is composed by two parts: 

- A sub-stream for representing the program's numerical output that is used in found-off error estimation. 

- A sub-stream for representing the program's signature (discrete feature) that is used in divergence detection. 

The output stream is in one the two formats described as follows. 

- The format for round-off error estimation only. 

| **value index** | description | 
| -------- | -------- | 
| 0 | An integer N (but is converted to a 128-bit floating-point number) which denotes that the following N numbers represent the program's numerical output. The following N numbers are used in floating-point round-off error estimation. |
| 1 | output representing value | 
| ... | ... | 
| N | output representing value |

- The format for divergence detection. 

| **value index** | description | 
| -------- | -------- | 
| 0 | An integer N (but is converted to a 128-bit floating-point number) which denotes that the following N numbers represent the program's numerical output. The following N numbers are used in floating-point round-off error estimation. |
| 1 | output representing value | 
| ... | ... | 
| N | output representing value |
| N+1 | A **positive** integer M (converted to 128-bit floating-point number) which denotes that the following M numbers represent the program's signature (discrete feature). The following M numbers are used in floating-point divergence detection. | 
| N+2 | signature representing value |
| ... | ... |
| N+1+M | signature representing value | 

#### I/O Consistency between the Programs and Their Real-number Versions 
At this point, we rely on the users to make sure that their programs and the corresponding real-number versions generate the same length of the numerical outputs and the same length of the signatures. 

#### Divergence Detection 
At this point, we rely on users to export **useful** information encoded in the signature part of the output stream for S3FP to detect divergence. 
For example, considering a variance calculation program that a divergence is returning a negative variance, the program should check the sign of the calculated variance and report the checking result to S3FP (in an integer of indicating the sign). 
S3FP should be (must be) able to judge whether a divergence occuring or not by only observing the signatures returned from both floating-point and real number executions. 


### Configuration File
S3FP requies a configuration file that 

- it needs to be named as "**s3fp_setting**" and 

- it needs to be placed at the current working directory. 

The format of the config. file is a list of lines that each of them satisfying the following syntax: 

```
[Option] = [Value]
```
The options and their available values are listed as follows: 

- **RT**: the option of input searching strategy. 
It can be one of the following strings: 
    * **URT**: unguided random testing, which is pure random testing
    * **BGRT**: binary guided random testing 
    * **ILS**: iterative local search 
    * **PSO**: partical swarm optimization 
    * **ABS**: abstract binary search. This is only used for divergence detection. 

- **TIMEOUT**: the option of timeout value. 
The value needs to be a **positive integer**. 
The meansing of the value is defined by option **RESOURCE**. 

- **RESOURCE**: the option of resource type. 
This option explains the meaning of the value specified by option **TIMEOUT**. 
It can be one of the following strings: 
    * **SVE**: the value of **TIMEOUT** specifies the number of input enumerations 
    * **TIME**: the value of **TIMEOUT** specifies the number of ellapse seconds 

- **RSEED**: the random number seed. 
The value needs to be a **non-negative integer**. 

- **N_VARS**: an **positive integer** which denotes the number of input values. 

- **INPUT_FILE**: the file name of the streaming input provided by S3FP. 

- **EXE_LP**: the original verison of the program which must be an executable. 

- **OUTPUT_LP**: the streaming output to S3FP. 

- **EXE_HP**: the real-number version of the program which mush be an executable. 

- **OUTPUT_HP**: the streaming output to S3FP. 

- **UNIFORM_INPUT**: a **boolean** indicating that whether all input values are in the same range specified by options UNIFORM_INPUTLB and UNIFORM_INPUTUB. 
If UNIFORM_INPUT = true, options UNIFORM_INPUTLB and UNIFORM_INPUTUB must be specified. 
Otherwise, option INPUT_RANGE_FILE must be specified. 

- **UNIFORM_INPUTLB**: a **floating-point number** which denotes the lower bound of **all** input values. 

- **UNIFORM_INPUTUB**: a **floating-point number"" which denotes the upper bound of **all** input values. 

- **INPUT_RANGE_FILE**: a **text file" of K lines for K input values. Each line denotes the range of a value and contains only two values separated by a space. The first value denotes the lower bound and the later value denotes the upper bound. 

- **ERR_FUNC**: a function maps accross the program's numerical output and, for each output value, computes the round-off error. 
Let L1, L2, ... LN is the stream of the program's numerical output, and H1, H2, ... HN is the stream of the program's real-number version's output. 
Let ef is the function specified by ERR_FUNC, values E1, E2, ... EN would be generated such that Ei = ef(Li, Hi). 
    * **REL**: computes the related error which is defined as ((Li - Hi) / max(abs(Li), REL_DELTA)). REL_DELTA is defined by another option. 
    * **ABSREL**: computes the absolute related error. 
    * **ABSRELMAXONE**: is defined as max(1, ABSREL). 
    * **ABS**: computes the absolute error which is defined as (Li - Hi). 
    * **ABSABS**: computes the absolute absolute error: |(Li - Hi)|. 
    * **LOWP**: is defined as Li. 

- **ERR_OPT**: a projection function for the program's error stream: E1, E2, ... EN. 
(Please refer to option ERR_FUNC for more details.) 
The error stream would be projected to a single value for the purpose of the comparison (between error streams). 
    * **FIRST**: is defined as E1. 
    * **LAST**: is defined as EN. 
    * **SUM**: is defined as sum(E1, E2, ... EN). 
    * **MAX**: is defined as max(E1, E2, ... EN). 
    * **AVE**: is defined as sum(E1, E2, ... EN) / N. 

- **REL_DELTA**: a **floating-point number** works as a padding when calculating the related error. 
The default value is 0. 

- **CHECK_DIV**: a **boolean** which indicates divergence detection. 

- **DIV_ERROR_REPORT**: a file that dedicate to S3FP for its internal use. 

- **SIG_FUNC**: the function that check the differential contract, which is 
**SIG_FUNC(Float's discrete results) == SIG_FUNC(Real's discrete results)**. 
    * **SINGLE_INT**: the signature should be a single integer. 
    * **SINGLE_UINT**: the signature should be a single **unsigned** integer. 
    * **LAST_INT**: the signature is a list of integers. But the list is projected to a single integer which is the last value of the list. 
    * **LAST_UINT**: the signature is a list of integers. But the list is projected to a single **unsigned** integer which is the last value of the list. 

- **DIV_FUNC**: the function that check divergence based on the program's discrete results. 
Again, at this point, we rely on programmers to export **useful** discrete results to S3FP such that S3FP can judge differential contract (the consistency between Float's and Real's signatures) and divergence by only observing the discrete results (from both Float and Real). The value of DIV_FUNC should be one of the candidate of SIG_FUNC. 


## Demo
----
A demo script, **s3fp-demo.py** is provided under directory **examples** which may help properly using S3FP. 
To use this script, the current working directory must be directory **examples**. 
The usage is shown as follows. 

```
s3fp-demo.py [ "round-off" | "div" ] [ benchmark name ] [ testing method ] [ random seed ] 
``` 

The usage details are shown in the following sections. 


### Demo of Round-off Error Estimation 
The usage is 

```
s3fp-demo.py round-off [ benchmark name ] [ testing method ] [ random seed ] 
```

- **benchmark name**: 

    * **balanced-reduction**: balanced reduction. 
    * **imbalanced-reduction**: imbalanced reduction. 
    * **kahan-sum**: Kahan's summation.  

- **testing method**: 

    * **URT**: pure random testing. 
    * **BGRT**: binary guided random testing. 
    * **ILS**: iterative local search. 
    * **PSO**: particle swarm optimization.   

- **random seed**: an unsigned integer. 


### Demo of Divergence Detection 
The usage is 

```
s3fp-demo.py div [ benchmark name ] [ testing method ] [ random seed ] 
```

- **benchmark name** which must be used with **ABS** as the testing method: 

    * **pc_3x3**: 2D point-to-circle judgement through a 3x3 matrix. 
    * **pc_4x4**: 2D point-to-circle judgement through a 4x4 matrix.
    * **pp_3x3**: 3D point-to-plane judgement through a 3x3 matrix. 
    * **pp_4x4**: 3D point-to-plane judgement through a 4x4 matrix. 
    * **ps_4x4**: 3D point-to-sphere judgement through a 4x4 matrix. 
    * **ps_5x5**: 3D point-to-sphere judgement through a 5x5 matrix.  
    * **inc_convex_hull**: 2D convex hull construction by using an incremental method. 

- **benchmark name** which must be used with **BGRT** as the testing method: 

    * **cpu-scan-naive-opt-last**: prefix-sum using naive approach and using the relative error of the last clause to drive BGRT's search. 
    * **cpu-scan-naive-opt-sum**: prefix-sum using naive approach and using the summation of the relative errors of all clauses to drive BGRT's search. 
    * **cpu-scan-harris-opt-last**: prefix-sum using the two-phase approach and using the relative error of the last clause to drive BGRT's search. 
    * **cpu-scan-harris-opt-sum**: prefix-sum using the two-phase approach and using the summation of the relative errors of all clauses to drive BGRT's search.     

- **testing method**: 
  
    * **BGRT**: binary guided random testing. 
    * **ABS**: abstract binary search. 
  
- **random seed**: an unsigned integer. 

