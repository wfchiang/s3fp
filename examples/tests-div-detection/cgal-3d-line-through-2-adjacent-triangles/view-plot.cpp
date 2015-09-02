#include <stdio.h>  
#include <sstream>
#include <assert.h>
extern "C" {
#include <quadmath.h> 
}

using namespace std; 

#ifndef IFT
#define IFT float 
#endif 


int main (int argc, char *argv[]) {
  assert(argc == 3); 

  FILE *infile = fopen(argv[1], "r");
  FILE *outfile = fopen(argv[2], "w"); 
  assert(infile != NULL);
  assert(outfile != NULL); 

  IFT Ax, Ay, Az; 
  IFT Bx, By, Bz; 
  IFT Cx, Cy, Cz; 
  IFT Dx, Dy, Dz; 
  IFT LXx, LXy, LXz; 
  IFT LYx, LYy, LYz; 

  fread(&Ax, sizeof(float), 1, infile);
  fread(&Ay, sizeof(float), 1, infile);
  fread(&Az, sizeof(float), 1, infile);

  fread(&Bx, sizeof(float), 1, infile);
  fread(&By, sizeof(float), 1, infile);
  fread(&Bz, sizeof(float), 1, infile);

  fread(&Cx, sizeof(float), 1, infile);
  fread(&Cy, sizeof(float), 1, infile);
  fread(&Cz, sizeof(float), 1, infile);

  fread(&Dx, sizeof(float), 1, infile);
  fread(&Dy, sizeof(float), 1, infile);
  fread(&Dz, sizeof(float), 1, infile);

  fread(&LXx, sizeof(float), 1, infile);
  fread(&LXy, sizeof(float), 1, infile);
  fread(&LXz, sizeof(float), 1, infile);

  fread(&LYx, sizeof(float), 1, infile);
  fread(&LYy, sizeof(float), 1, infile);
  fread(&LYz, sizeof(float), 1, infile);  

  fprintf(outfile, "\\documentclass{article} \n"); 
  fprintf(outfile, "\\usepackage{pgfplots} \n"); 
  fprintf(outfile, "\\usepackage{graphicx} \n"); 
  fprintf(outfile, "\\begin{document} \n"); 
  fprintf(outfile, "\\begin{itemize} \n"); 
  fprintf(outfile, "\\item A : (%3.2f, %3.2f, %3.2f) \n", Ax, Ay, Az); 
  fprintf(outfile, "\\item B : (%3.2f, %3.2f, %3.2f) \n", Bx, By, Bz); 
  fprintf(outfile, "\\item C : (%3.2f, %3.2f, %3.2f) \n", Cx, Cy, Cz); 
  fprintf(outfile, "\\item D : (%3.2f, %3.2f, %3.2f) \n", Dx, Dy, Dz); 
  fprintf(outfile, "\\item L : (%3.2f, %3.2f, %3.2f) -- (%3.2f, %3.2f, %3.2f) \n", LXx, LXy, LXz, LYx, LYy, LYz); 
  fprintf(outfile, "\\end{itemize} \n"); 
  fprintf(outfile, "\\resizebox{1\\linewidth}{!}{ \n"); 
  fprintf(outfile, "\\begin{tikzpicture} \n"); 
  fprintf(outfile, "\\begin{axis}[ xlabel=x axis, ylabel=y axis] \n"); 
  // fprintf(outfile, "\\addplot3 [ draw=blue, fill=blue ] coordinates { \n"); 
  fprintf(outfile, "\\addplot3 [ draw=blue, thick=3 ] coordinates { \n"); 
  fprintf(outfile, "  (%8.7f, %8.7f, %8.7f) \n", Ax, Ay, Az); 
  fprintf(outfile, "  (%8.7f, %8.7f, %8.7f) \n", Bx, By, Bz); 
  fprintf(outfile, "  (%8.7f, %8.7f, %8.7f) \n", Cx, Cy, Cz); 
  fprintf(outfile, "  (%8.7f, %8.7f, %8.7f) }; \n", Ax, Ay, Az); 
  fprintf(outfile, "\\addplot3 [ draw=black, thick=3 ] coordinates { \n");
  fprintf(outfile, "  (%8.7f, %8.7f, %8.7f) \n", LXx, LXy, LXz); 
  fprintf(outfile, "  (%8.7f, %8.7f, %8.7f) }; \n", LYx, LYy, LYz); 
  // fprintf(outfile, "\\addplot3 [ draw=red, fill=red ] coordinates { \n"); 
  fprintf(outfile, "\\addplot3 [ draw=red, thick=3 ] coordinates { \n"); 
  fprintf(outfile, "  (%8.7f, %8.7f, %8.7f) \n", Ax, Ay, Az); 
  fprintf(outfile, "  (%8.7f, %8.7f, %8.7f) \n", Bx, By, Bz); 
  fprintf(outfile, "  (%8.7f, %8.7f, %8.7f) \n", Dx, Dy, Dz); 
  fprintf(outfile, "  (%8.7f, %8.7f, %8.7f) }; \n", Ax, Ay, Az); 
  fprintf(outfile, "\\end{axis} \n"); 
  fprintf(outfile, "\\end{tikzpicture} } \n"); 
  fprintf(outfile, "\\end{document} \n"); 

  fclose(infile); 
  fclose(outfile); 

  stringstream ss; 
  ss << "pdflatex " << argv[2]; 
  system(ss.str().c_str()); 
  
  return 0; 
}

