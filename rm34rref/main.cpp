  // The goal is to compute RM32.rref_signature
 // and save it to a file
// for later processing for RM64.pivot_signature

#include<omp.h>
#include<stdio.h>
#include<stdlib.h>
#define thirtytwo 32
#define five 5

int binomial5choose[five+1]={1,5,10,10,5,1};
int sumafter5choose[five+1]={  5+10+10+5+1,
                                 10+10+5+1,
                                    10+5+1,
                                       5+1,
                                         1,
                                         0};

using namespace std;
#include<array>
typedef array<uint16_t,thirtytwo> rref32asarray_t;
typedef struct{
    u_int16_t block1_____[1];
    u_int16_t block_5____[5];
    u_int16_t block__a___[10];
    u_int16_t block___a__[10];
    u_int16_t block____5_[5];
    u_int16_t block_____1[1];
    u_int16_t*operator[](int block){
        switch(block){
            case 0:return block1_____;
            case 1:return block_5____;
            case 2:return block__a___;
            case 3:return block___a__;
            case 4:return block____5_;
            case 5:return block_____1;
            default:exit(56672);
        }
    };
}rref32byblock_t;
union rref32_t{
    rref32asarray_t array;
    rref32byblock_t block;
};

typedef int32_t pattern32_t;
typedef int64_t pattern64_t;

// our beloved pivot/rref signature polynomial is an std::map
#include<map>
typedef map<rref32asarray_t,uint32_t> Poly32_t;

// declare how to add polynomial in C++
Poly32_t&Poly32_add(Poly32_t&left,Poly32_t&right){
    for(const auto&piv_coe_pair:right){
        left[piv_coe_pair.first]+=piv_coe_pair.second;
    }
    right.clear();
    return left;
}
// declare how to add polynomial again for openmp
#pragma omp declare reduction(omp_poly32_add:Poly32_t:omp_out=Poly32_add(omp_out,omp_in))


// Very Efficient Gaussian Elimination
pattern32_t GaussianElimination(uint32_t*WorkingMatrix,int height,int width){
    //                                   ^                 ^the dimension of the matrix
    //                                   ^                  need to be boundary-safe
    //                                   ^the working space as a pointer
    //                                    resulting RREF will be here
    int rank_acc=0; // tracking the number of pivots while doing RREF = rank in matroid language
    pattern32_t pivot_pattern=0; // tracking pivot pattern;
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
        uint32_t the_pivot_row=WorkingMatrix[pivot_found_at];  // swap !!
        WorkingMatrix[pivot_found_at]=WorkingMatrix[rank_acc]; // swap !!
        WorkingMatrix[rank_acc]=the_pivot_row;                 // swap !!
        rank_acc+=1;
        pivot_pattern|=1<<col;
        // eliminate upstairs
        for(int row=0;row<rank_acc-1;row++){ // handle this column
            if(WorkingMatrix[row]&(1<<col)){ // if need to eliminate this row
                WorkingMatrix[row]^=the_pivot_row;
            }
        }
        // eliminate downstairs
        for(int row=pivot_found_at+1;row<height;row++){ // handle this column
            if(WorkingMatrix[row]&(1<<col)){ // if need to eliminate this row
                WorkingMatrix[row]^=the_pivot_row;
            }
        }
    }
    return pivot_pattern;
}

int main (int argc, const char *argv[]) {
    printf(R"rm32(                          ___             __  )rm32""\n");
    printf(R"rm32(                         / _ \___ ___ ___/ /  )rm32""\n");
    printf(R"rm32(                        / , _/ -_) -_) _  /   )rm32""\n");
    printf(R"rm32(                       /_/|_|\__/\__/\_,_/    )rm32""\n");
    printf(R"rm32(                      /  |/  /_ __/ / /__ ____)rm32""\n");
    printf(R"rm32(                     / /|_/ / // / / / -_) __/)rm32""\n");
    printf(R"rm32(                    /_/  /_/\_,_/_/_/\__/_/   )rm32""\n");
    printf(R"rm32(                              / __// / /      )rm32""\n");
    printf(R"rm32(                             / _ \/_  _/      )rm32""\n");
    printf(R"rm32(                             \___/ /_/        )rm32""\n");
    printf(R"rm32(                                              )rm32""\n");
    
    //      double check sizeof
    printf("double check sizeof \n");
    printf("    sizeof(rref32asarray_t)=%lu \n",sizeof(rref32asarray_t));
    printf("    sizeof(rref32byblock_t)=%lu \n",sizeof(rref32byblock_t));
    printf("    sizeof(rref32_t       )=%lu \n",sizeof(rref32_t       ));
    
    // prepare matrix
    int monomials_in_order[thirtytwo]={31,30,29,27,23,15,28,26,22,
        14,25,21,13,19,11,7,24,20,12,18,10,6,17,9,5,3,16,8,4,2,1,0};
    int ReedMullerMother[thirtytwo]={};
    for(int row=0;row<thirtytwo;row++){
        for(int col=thirtytwo-1;col>=0;col--){ // col for column
            int mon=monomials_in_order[col];
            ReedMullerMother[row]+=int((row&mon)==0)<<col;
        }
    }
    //      double check monomial order
    printf("double check monomial order \n");
    for (int dig=five-1;dig>=0;dig--){
        printf("   ");
        for(int col=thirtytwo-1;col>=0;col--){
            printf(monomials_in_order[col]&(1<<dig)?" #":" -");
        }
        printf("\n");
    }
    //      double check the matrix
    printf("double check the matrix \n");
    for (int row=0;row<thirtytwo;row++){
        printf("   ");
        for(int col=thirtytwo-1;col>=0;col--){
            printf(ReedMullerMother[row]&(1<<col)?" #":" -");
        }
        printf("\n");
    }
    printf("caution: u-vector is multiplied from right \n");
    printf("caution: channel is going to erase some ROWs \n");
    printf("\n");
    
    printf("start timing \n");
    double wtime_start=omp_get_wtime();
    
    
    
    
    
    int max_threads=omp_get_max_threads();
    printf(    "    omp_get_max_threads() returns %d \n",max_threads);
    printf(    "    omp_set_num_threads(%d)          \n",max_threads);
                    omp_set_num_threads(                 max_threads);
    
    //          start the rref loop
    printf("    start the rref loop \n");
    printf("    ");
    Poly32_t RREF_Signature_Counter; // our purpose is to compute this
    // exhaus all erasure pattern
    #pragma omp parallel for reduction(omp_poly32_add:RREF_Signature_Counter)
    for(pattern64_t row_era_pat=(1L<<thirtytwo)-1;row_era_pat>=0;row_era_pat-=1234){ // era for erasure pattern
        if(row_era_pat%(1<<20)==0)printf("%lld ",row_era_pat>>20);
        if(row_era_pat%(1<<26)==0)printf("\n    ");
        // copy the preserved matrix to working space
        int cardinality_acc=0; // tracking the number of rows that are preserved when erasing (i.e. cardinality in matroid language)
        uint32_t RMpreserved[thirtytwo+1]={}; // preserved rows go to here, the working space.
        //+1 because when compressing, need an empty room
        for(int row=0;row<thirtytwo;row++){ // check row by row
            if((row_era_pat&(1<<row))==0){ // if row-th row is preserved (i.e. if not erased)
                RMpreserved[cardinality_acc]=ReedMullerMother[row];
                cardinality_acc+=1;
            }
        }
        
        GaussianElimination(RMpreserved,cardinality_acc,thirtytwo);
        
        // new a rref holder to hold the result
        rref32_t rref_holder;
        for(int i=0;i<thirtytwo;i++){
            rref_holder.array[i]=0;
        }
        // compress rows into blocks
        for(int row=0,block=0;block<=five;block++){ // operate per block
            for(int subrow=0;RMpreserved[row]>>sumafter5choose[block];subrow++,row++){
                rref_holder.block[block][subrow]=RMpreserved[row]>>sumafter5choose[block];
                assert(subrow<binomial5choose[block]);
                assert(row<thirtytwo);
            }
        }
        // push rref_result into the map
        RREF_Signature_Counter[rref_holder.array]+=1;
    }
    printf("\n");
    printf("    exit the rref loop \n");
    printf("\n");
    
    //          turn counter into array
    printf("    turn counter into array \n");
    int counter_size=(int)RREF_Signature_Counter.size();
    printf("    rref signature polynomial has size %d \n",counter_size);
    rref32_t*RREF_Signature_Array_rref=new rref32_t[counter_size];
    uint32_t*RREF_Signature_Array_coef=new uint32_t[counter_size];
    {
        int term=0;
        for(const auto&rref_coef_pair:RREF_Signature_Counter){
            RREF_Signature_Array_rref[term].array=rref_coef_pair.first;
            RREF_Signature_Array_coef[term]      =rref_coef_pair.second;
            term++;
        }
    }
    RREF_Signature_Counter.clear();
    
    
    char RREFSignatureFilename[128];
    sprintf(RREFSignatureFilename,"rm32rref.txt");
    printf("    rref signature polynomial goes to %s \n",RREFSignatureFilename);
    {
        FILE*RREFSignatureFilehook;
        RREFSignatureFilehook=fopen(RREFSignatureFilename,"w+");
        int coefficient_acc=0; // just for sanity check
        for(int term=0;term<counter_size;term++){
            rref32asarray_t rref_result=RREF_Signature_Array_rref[term].array;
            uint32_t coefficient=RREF_Signature_Array_coef[term];
            for(int i=0;i<thirtytwo;i++){
                u_int16_t result_i=rref_result[i];
                fprintf(RREFSignatureFilehook,"%03x ",result_i);
            }
            fprintf(RREFSignatureFilehook,"%4x \n",coefficient);
            coefficient_acc+=coefficient;
        }
        fclose(RREFSignatureFilehook);
        printf("    (sanity check) total coefficient is %d \n",coefficient_acc);
    }
    printf("\n");
    
    
    
    
    double wtime_delta=omp_get_wtime()-wtime_start;
    printf("end timing \n");
    printf("wtime_delta is %.3g seconds \n",wtime_delta);
    printf("\n");
    
    
    printf("\n");
    printf("Thank you so much-a for-to playing my game \n");
    printf("\n");
    printf("\n");
}

// ASCII from
// http://patorjk.com/software/taag/#p=display&f=Small%20Slant&t=%20%20%20%20%20Reed%0A%20%20Muller%0A%20%20%20%20%20%20%20%20%20%2064

// self-defined arr[i] in struct
// https://stackoverflow.com/questions/37043078/c-overloading-array-operator
