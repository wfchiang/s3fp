S3FP
====



About S3FP
----------
S3FP is a dynamic testing tool for triggering high floating-point error scenarios of programs. 
Currently, the high floating-point error scenarios include: 

- high round-off error occuring on a variable

- divergence (discrete value deviation) 

The error scenarios are triggered with witnessing inputs. 
Such inputs can be useful in various purposes such as program analysis and tuning. 

For more information, please refer to the related publications: 

- Wei-Fan Chiang, Ganesh Gopalakrishnan, Zvonimir Rakamaric, and Alexey Solovyev. "Efficient search for inputs causing high floating-point errors". In PPoPP, 2014. 



Installation
------------
Please just use the python script "s3fp-install.py" for installation by 

```
s3fp-install.py build
```


Usage
-----
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

