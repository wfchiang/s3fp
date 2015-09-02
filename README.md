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
**s3fp-install.py build** 
```
