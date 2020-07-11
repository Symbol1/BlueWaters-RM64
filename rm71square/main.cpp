  // The goal is to compute RM64.pivot_signature
 // by the RM32.rref_signature we had done
// Use MPI to do a lot of things at once

#include<mpi.h>
#include<omp.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

#define thirtytwo 32
#define five 5
#define fiveseventwoninefive 57295
#define threeoneone 311
#define onefivesix 156

const int binomial5choose[five+1]={1,5,10,10,5,1};
const int sumafter5choose[five+1]={
    5+10+10+5+1+0,
      10+10+5+1+0,
         10+5+1+0,
            5+1+0,
              1+0,
                0};

typedef u_int16_t matrixrow10_t;
typedef u_int32_t matrixrow20_t;
typedef u_int64_t pattern64_t;


// rm32 rref comes in two forms
// first the array form
using namespace std;
#include<array>
typedef array<matrixrow10_t,thirtytwo> rref32asarray_t;
// second the block form
typedef struct rref32byblock_t{
    matrixrow10_t block1_____[1];
    matrixrow10_t block_5____[5];
    matrixrow10_t block__a___[10];
    matrixrow10_t block___a__[10];
    matrixrow10_t block____5_[5];
    matrixrow10_t block_____1[1];
    matrixrow10_t*operator[](int block){
        switch(block){
                case 0:return block1_____;
                case 1:return block_5____;
                case 2:return block__a___;
                case 3:return block___a__;
                case 4:return block____5_;
                case 5:return block_____1;
            default:exit(48442);
        }
    };
}rref32byblock_t;
// unify them
union rref32_t{
    rref32asarray_t array;
    rref32byblock_t block;
};
typedef u_int64_t coef64_t;


// our beloved pivot/rref signature polynomial is an std::map
typedef struct rref_coef_pair_t{
    rref32_t rref;
    coef64_t coef;
}rref_coef_pair_t;
typedef rref_coef_pair_t Poly32_t[fiveseventwoninefive];
#include<map>
typedef map<pattern64_t,coef64_t> Poly64_t;

// declare how to add polynomial in C++
Poly64_t&Poly64_add(Poly64_t&left,Poly64_t&right){
    for(const auto&piv_coe_pair:right){
        left[piv_coe_pair.first]+=piv_coe_pair.second;
    }
    right.clear();
    return left;
}
// declare how to add polynomial again for openmp
#pragma omp declare reduction(omp_poly64_add:Poly64_t:omp_out=Poly64_add(omp_out,omp_in))






// Very Efficient Gaussian Elimination
pattern64_t GaussianElimination(matrixrow20_t*WorkingMatrix,int height,int width){
    //                                       ^                 ^the dimension of the matrix
    //                                       ^                  need to be boundary-safe
    //                                       ^the working space as a pointer
    //                                        resulting RREF will be here
    int rank_acc=0; // tracking the number of pivots while doing RREF = rank in matroid language
    pattern64_t pivot_pattern=0; // tracking pivot pattern;
    for(int col=width-1;col>=0;col--){ // loop for columns (i.e. positions of pivots)
        int pivot_found_at=-404; // default to -404 // might cause bug if positive
        for(int row=rank_acc;row<height;row++){ // find the pivot
            if((WorkingMatrix[row]&(1<<col))){ // found the pivot
                pivot_found_at=row; // the pivot is at this row
                break;
            }
        }
        if(pivot_found_at==-404){ // no pivot for this column // try next column
            continue;
        }
        // found the pivot
        // swap to top
        matrixrow20_t the_pivot_row=WorkingMatrix[pivot_found_at]; // swap !!
        WorkingMatrix[pivot_found_at]=WorkingMatrix[rank_acc];     // swap !!
        WorkingMatrix[rank_acc]=the_pivot_row;                     // swap !!
        rank_acc+=1;
        pivot_pattern|=1<<col;
        // eliminate upstairs // not necessary in this stage
        // eliminate downstairs
        for(int row=pivot_found_at+1;row<height;row++){ // handle this column
            if(WorkingMatrix[row]&(1<<col)){ // if need to eliminate this row
                WorkingMatrix[row]^=the_pivot_row;
            }
        }
    }
    return pivot_pattern;
}







int main(){


    // MPI has highest priority
    MPI_Init(NULL,NULL);
    int comm_size;
    MPI_Comm_size(MPI_COMM_WORLD ,&comm_size);
    int comm_rank;
    MPI_Comm_rank(MPI_COMM_WORLD ,&comm_rank);
    printf("MPI rank %d / size %d \n",comm_rank,comm_size);

    MPI_Barrier(MPI_COMM_WORLD);

    if(comm_rank==0){
        printf(R"rm64(                          ___             __  )rm64""\n");
        printf(R"rm64(                         / _ \___ ___ ___/ /  )rm64""\n");
        printf(R"rm64(                        / , _/ -_) -_) _  /   )rm64""\n");
        printf(R"rm64(                       /_/|_|\__/\__/\_,_/    )rm64""\n");
        printf(R"rm64(                      /  |/  /_ __/ / /__ ____)rm64""\n");
        printf(R"rm64(                     / /|_/ / // / / / -_) __/)rm64""\n");
        printf(R"rm64(                    /_/  /_/\_,_/_/_/\__/_/   )rm64""\n");
        printf(R"rm64(                              / __// / /      )rm64""\n");
        printf(R"rm64(                             / _ \/_  _/      )rm64""\n");
        printf(R"rm64(                             \___/ /_/        )rm64""\n");
        printf(R"rm64(                                              )rm64""\n");
    }

    MPI_Barrier(MPI_COMM_WORLD);





    printf("rank %d: get wtime big \n",comm_rank);
    double           get_wtime_big=omp_get_wtime();


    // the loop for different pair of subtxt
    for(int subtxt_no=comm_rank;subtxt_no<1;subtxt_no+=comm_size){
        int subtxt_no_TTT=(24+subtxt_no/onefivesix)%threeoneone;
        int subtxt_no_SSS=(subtxt_no_TTT+subtxt_no%onefivesix)%threeoneone;

        printf("    %03d x %03d: get wtime small \n",subtxt_no_TTT,subtxt_no_SSS);
        double                   get_wtime_small=omp_get_wtime();

        // set up omp, will use this to determine where am I
        int max_threads=omp_get_max_threads();
        omp_set_num_threads(max_threads);
        
        // read subtxt, the ttt term
        Poly32_t RREF_Signature_TTT;
        coef64_t coefficient_ttt_acc=0;
        {
            FILE*RREFSignatureFilehook_TTT;
            char    RREFSignatureFilename_TTT[1024];
            if(max_threads==8){
                sprintf(RREFSignatureFilename_TTT,"/Users/home/SageMath/rm32/rm35rref/rm35rref%03d.txt",subtxt_no_TTT);
            }else{
                sprintf(RREFSignatureFilename_TTT,   "/projects/sciteam/baus/rm35rref/rm35rref%03d.txt",subtxt_no_TTT);
            }
            RREFSignatureFilehook_TTT=fopen(RREFSignatureFilename_TTT,"r");
            for(int term=0;term<fiveseventwoninefive;term++){
                for(int i=0;i<thirtytwo;i++){
                    fscanf(RREFSignatureFilehook_TTT,"%hx",&RREF_Signature_TTT[term].rref.array[i]);
                }
                fscanf(RREFSignatureFilehook_TTT,"%llx",&RREF_Signature_TTT[term].coef);
                coefficient_ttt_acc+=RREF_Signature_TTT[term].coef;
            }
            fclose(RREFSignatureFilehook_TTT);
        }

        // read subtxt, the sss serm
        Poly32_t RREF_Signature_SSS;
        coef64_t coefficient_sss_acc=0;
        {
            FILE*RREFSignatureFilehook_SSS;
            char    RREFSignatureFilename_SSS[1024];
            if(max_threads==8){
                sprintf(RREFSignatureFilename_SSS,"/Users/home/SageMath/rm32/rm35rref/rm35rref%03d.txt",subtxt_no_SSS);
            }else{
                sprintf(RREFSignatureFilename_SSS,   "/projects/sciteam/baus/rm35rref/rm35rref%03d.txt",subtxt_no_SSS);
            }
            RREFSignatureFilehook_SSS=fopen(RREFSignatureFilename_SSS,"r");
            for(int serm=0;serm<fiveseventwoninefive;serm++){
                for(int i=0;i<thirtytwo;i++){
                    fscanf(RREFSignatureFilehook_SSS,"%hx",&RREF_Signature_SSS[serm].rref.array[i]);
                }
                fscanf(RREFSignatureFilehook_SSS,"%llx",&RREF_Signature_SSS[serm].coef);
                coefficient_sss_acc+=RREF_Signature_SSS[serm].coef;
            }
            fclose(RREFSignatureFilehook_SSS);
        }

        // start the rref-pair loop
        Poly64_t Pivot_Signature_Counter; // our purpose is to compute this
        // exhaus all ordered pair
        #pragma omp parallel for reduction(omp_poly64_add:Pivot_Signature_Counter)
        for(int term=0;term<fiveseventwoninefive;term+=1){
            for(int serm=0;serm<fiveseventwoninefive;serm+=1){
                rref32byblock_t TERM=RREF_Signature_TTT[term].rref.block;
                rref32byblock_t SERM=RREF_Signature_SSS[serm].rref.block;
                pattern64_t pivot_pattern=0;
                for(int block=0;block<=five;block++){
                    matrixrow20_t PlotkinBlock[2*binomial5choose[block]]; // claim working space
                    // no need to initialize because we will copy data and keep track of size
                    int rank_acc=0;
                    for(int subrow=0;subrow<binomial5choose[block]&&TERM[block][subrow];subrow++){
                        PlotkinBlock[rank_acc]=TERM[block][subrow]*(1<<binomial5choose[block]);
                        rank_acc++;
                    }
                    for(int subrow=0;subrow<binomial5choose[block]&&SERM[block][subrow];subrow++){
                        PlotkinBlock[rank_acc]=SERM[block][subrow]*(1<<binomial5choose[block]|1);
                        rank_acc++;
                    }
                    pattern64_t pivot_subpat=GaussianElimination(PlotkinBlock,rank_acc,2*binomial5choose[block]);
                    pivot_pattern|=pivot_subpat<<(2*sumafter5choose[block]);
                }
                Pivot_Signature_Counter[pivot_pattern]+=RREF_Signature_TTT[term].coef*RREF_Signature_SSS[serm].coef;
            }
        }



        {
            FILE*PivotSignatureFilehook;
            char PivotSignatureFilename[1024];
            sprintf(PivotSignatureFilename,"rm64pivot%03dx%03d.txt",subtxt_no_TTT,subtxt_no_SSS);
            printf("    %03d x %03d:     open %s \n",subtxt_no_TTT,subtxt_no_SSS,PivotSignatureFilename);
            PivotSignatureFilehook=fopen(PivotSignatureFilename,"w+");
            coef64_t coefficient_acc=0; // just for sanity check
            for(const auto&piv_coe_pair:Pivot_Signature_Counter){
                fprintf(PivotSignatureFilehook,"%016llx %16llx \n",piv_coe_pair.first,piv_coe_pair.second);
                coefficient_acc+=piv_coe_pair.second;
            }
            fclose(PivotSignatureFilehook);
            printf("    %03d x %03d:     closed; sanity check:",subtxt_no_TTT,subtxt_no_SSS);
            printf("    %llu - %llu = %llu \n",coefficient_acc,coefficient_ttt_acc*coefficient_sss_acc,
                                               coefficient_acc-coefficient_ttt_acc*coefficient_sss_acc);
        }

        double                   delta_wtime_small=omp_get_wtime()-get_wtime_small;
        printf("    %03d x %03d: delta_wtime_small is %.3g seconds \n",subtxt_no_TTT,subtxt_no_SSS,delta_wtime_small);

    }

    double           delta_wtime_big=omp_get_wtime()-get_wtime_big;
    printf("rank %d: delta_wtime_big is %.3g seconds \n",comm_rank,delta_wtime_big);


    printf("\n");
    printf("Thank you so much-a for-to playing my game \n");
    printf("\n");
    printf("\n");


    MPI_Finalize();
}

// ASCII from
// http://patorjk.com/software/taag/#p=display&f=Small%20Slant&t=%20%20%20%20%20Reed%0A%20%20Muller%0A%20%20%20%20%20%20%20%20%20%2064

// MPI setup from
// https://stackoverflow.com/questions/12593524/how-to-install-openmpi-for-xcode
// http://people.clarkson.edu/~bhelenbr/Research_Pages/Xcode_%26_MPI.html
// https://stackoverflow.com/questions/9388621/how-do-i-compile-openmpi-programs-using-xcode-4
    // I did not add libmpi_cxx.1.dylib because I cannot find it
// https://stackoverflow.com/questions/12593524/how-to-install-openmpi-for-xcode

// MPI I/O
// http://wgropp.cs.illinois.edu/courses/cs598-s16/lectures/lecture32.pdf

// do not forget --wdir

