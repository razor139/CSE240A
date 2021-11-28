//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Mohit Shah";
const char *studentID   = "A59005444";
const char *email       = "m1shah@ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[5] = { "Static", "Gshare",
                          "Tournament", "Custom", "Custom1" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//
// Gshare
uint32_t *gshare_pht;
uint32_t gshare_ghr;
uint32_t gshare_hist_mask;

// Tournament 
uint32_t *tour_local_ht;
uint32_t *tour_local_predt;
uint32_t *tour_global_predt;
uint32_t *tour_choice_predt;
uint32_t tour_ghr;
uint32_t tour_local_hist_mask;
uint32_t tour_global_hist_mask;
uint32_t pc_index_mask;

// Custom
uint32_t *local_bht;
uint32_t **custom_pht;
uint32_t custom_ghr;
uint32_t custom_local_hist_mask;
uint32_t custom_global_hist_mask;
uint32_t custom_pc_index_mask;

//Custom1

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

uint32_t create_mask(int bit_len){
    uint32_t mask = 0;
    for (int i =0; i < bit_len;i++){
        mask = mask | ( 1 << i );
    }
    return mask;
}
// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
  int size,local_size,local_hist_size;
  gshare_hist_mask = create_mask(ghistoryBits);
  tour_global_hist_mask = create_mask(ghistoryBits);
  tour_local_hist_mask = create_mask(lhistoryBits);
  pc_index_mask = create_mask(pcIndexBits);

  gshare_ghr = 0;
  tour_ghr = 0;
  custom_ghr = 0;

  switch (bpType) {
      case STATIC:
      case GSHARE:
        size = 1 << ghistoryBits;
        gshare_pht = (uint32_t*) malloc(size*sizeof(uint32_t));

        for (int i = 0; i < size;i++){
            gshare_pht[i] = 1;
        }
      case TOURNAMENT:
        size = 1 << ghistoryBits;
        local_size = 1 << lhistoryBits;
        local_hist_size = 1 << pcIndexBits;

        tour_global_predt = (uint32_t*) malloc(size*sizeof(uint32_t));
        tour_choice_predt = (uint32_t*) malloc(size*sizeof(uint32_t));
        tour_local_predt  = (uint32_t*) malloc(local_size*sizeof(uint32_t));
        tour_local_ht     = (uint32_t*) malloc(local_hist_size*sizeof(uint32_t));

        for (int i = 0; i < size;i++){
          tour_global_predt[i] = 1;
        }

        for (int i = 0; i < size;i++){
          tour_choice_predt[i] = 1;
        }

        for (int i = 0; i < local_size;i++){
          tour_local_predt[i] = 1;
        }

        for (int i = 0; i < local_hist_size;i++){
          tour_local_ht[i] = 0;
        }

      case CUSTOM:
        ghistoryBits = 9;
        lhistoryBits = 5;
        pcIndexBits  = 12;
        custom_local_hist_mask = create_mask(lhistoryBits);
        custom_global_hist_mask = create_mask(ghistoryBits);
        custom_pc_index_mask = create_mask(pcIndexBits);

        size = 1 << ghistoryBits;
        local_size = 1 << lhistoryBits;
        local_hist_size = 1 << pcIndexBits;

        local_bht  = (uint32_t*) malloc(local_hist_size*sizeof(uint32_t));
        for (int i =0; i < local_hist_size;i++){
           local_bht[i] = 0;
        }

        custom_pht = (uint32_t**) malloc(local_size*sizeof(uint32_t*));
        for (int i = 0;i < local_size;i++){
          custom_pht[i] = (uint32_t*) malloc(size*sizeof(uint32_t));
        }

        for (int i = 0;i < local_size;i++){
          for (int j = 0;j < size;j++){
            custom_pht[i][j] = 1;
          }
        }

      default:
        break;
    }



}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //
  uint32_t prediction;
  // Gshare
  uint32_t pc_select;
  uint32_t gshare_select;
  uint32_t gshare_pht_index;

  // Tournament
  uint32_t tour_pc_index;
  uint32_t tour_choice_index;
  uint32_t tour_global_index;
  uint32_t tour_local_index;
  uint32_t local_prediction;
  uint32_t global_prediction;
  uint32_t choice_value;

  // Custom
  uint32_t custom_pht_index;
  uint32_t custom_pht_sel;
  uint32_t custom_pc_pht_index;
  uint32_t custom_pc_bht_index;
  uint32_t custom_ghr_sel;
  // Make a prediction based on the bpType
  
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case GSHARE:
        pc_select = pc & gshare_hist_mask;
        gshare_select = gshare_ghr & gshare_hist_mask;
        gshare_pht_index = gshare_select ^ pc_select;

        prediction = gshare_pht[gshare_pht_index];
        
        if (prediction > 1){
            return TAKEN;
        }
        else return NOTTAKEN;

    case TOURNAMENT:
       tour_pc_index = pc & pc_index_mask;
       tour_global_index = tour_ghr & tour_global_hist_mask;
       tour_choice_index = tour_global_index;
       choice_value      = tour_choice_predt[tour_choice_index];

       global_prediction = tour_global_predt[tour_global_index];

       tour_local_index  = tour_local_ht[tour_pc_index] & tour_local_hist_mask;
       local_prediction  = tour_local_predt[tour_local_index];

       if (choice_value < 2) prediction = global_prediction;
       else prediction = local_prediction;

       if (prediction > 1) return TAKEN;
       else return NOTTAKEN;

    case CUSTOM:
      custom_pc_bht_index = pc & custom_pc_index_mask;
      custom_pc_pht_index = pc & custom_global_hist_mask;
      custom_ghr_sel  = custom_ghr & custom_global_hist_mask;
      custom_pht_index = custom_pc_pht_index ^ custom_ghr_sel;
      
      custom_pht_sel = local_bht[custom_pc_bht_index] & custom_local_hist_mask;
      //custom_pht_sel = (local_bht[custom_pc_bht_index] & custom_local_hist_mask) ^ (pc & custom_local_hist_mask);

      prediction = custom_pht[custom_pht_sel][custom_pht_index];

      if (prediction > 1) return TAKEN;
      else return NOTTAKEN;

    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //
  uint32_t prediction;
  // Gshare
  uint32_t pc_select;
  uint32_t gshare_select;
  uint32_t gshare_pht_index;

  // Tournament
  uint32_t tour_pc_index;
  uint32_t tour_choice_index;
  uint32_t tour_global_index;
  uint32_t tour_local_index;
  uint32_t local_prediction;
  uint32_t global_prediction;
  uint32_t local_pred_final;
  uint32_t global_pred_final;
  uint32_t choice_value;

  // Custom
  uint32_t custom_pht_index;
  uint32_t custom_pht_sel;
  uint32_t custom_pc_pht_index;
  uint32_t custom_pc_bht_index;
  uint32_t custom_ghr_sel;

  switch (bpType) {
      case STATIC:
      case GSHARE:
          pc_select = pc & gshare_hist_mask;
          gshare_select = gshare_ghr & gshare_hist_mask;
          gshare_pht_index = gshare_select ^ pc_select;

          prediction = gshare_pht[gshare_pht_index];

          if (outcome == 1){
              if (prediction != 3) gshare_pht[gshare_pht_index]++;
              gshare_ghr = (gshare_ghr << 1) | 1;
          }
          else {
              if (prediction != 0) gshare_pht[gshare_pht_index]--;
              gshare_ghr = (gshare_ghr << 1);
          }

          
      case TOURNAMENT:
          tour_pc_index = pc & pc_index_mask;
          tour_global_index = tour_ghr & tour_global_hist_mask;
          tour_choice_index = tour_global_index;
          choice_value      = tour_choice_predt[tour_choice_index];

          global_prediction = tour_global_predt[tour_global_index];

          tour_local_index  = tour_local_ht[tour_pc_index] & tour_local_hist_mask;
          local_prediction  = tour_local_predt[tour_local_index];

          if (local_prediction > 1) local_pred_final = TAKEN;
          else local_pred_final = NOTTAKEN;

          if (global_prediction > 1) global_pred_final = TAKEN;
          else global_pred_final = NOTTAKEN;

          if (outcome == TAKEN){
            if (global_prediction != 3) tour_global_predt[tour_global_index]++;
            if (local_prediction != 3)  tour_local_predt[tour_local_index]++;
          }
          else {
            if (global_prediction != 0) tour_global_predt[tour_global_index]--;
            if (local_prediction != 0)  tour_local_predt[tour_local_index]--;
          }

          if ((choice_value > 0) && (global_pred_final == outcome) && (local_pred_final != outcome)){
            tour_choice_predt[tour_choice_index]--;
          }
          else if((choice_value < 3) && (global_pred_final != outcome) && (local_pred_final == outcome)){
            tour_choice_predt[tour_choice_index]++;
          }

          tour_ghr = (tour_ghr << 1) | outcome;
          tour_local_ht[tour_pc_index] = (tour_local_ht[tour_pc_index] << 1) | outcome;
      case CUSTOM:
          custom_pc_bht_index = pc & custom_pc_index_mask;
          custom_pc_pht_index = pc & custom_global_hist_mask;
          custom_ghr_sel  = custom_ghr & custom_global_hist_mask;
          custom_pht_index = custom_pc_pht_index ^ custom_ghr_sel;
          
          custom_pht_sel = local_bht[custom_pc_bht_index] & custom_local_hist_mask;
          //custom_pht_sel = (local_bht[custom_pc_bht_index] & custom_local_hist_mask) ^ (pc & custom_local_hist_mask);

          prediction = custom_pht[custom_pht_sel][custom_pht_index];

          if (outcome == TAKEN){
            if (prediction < 3) custom_pht[custom_pht_sel][custom_pht_index]++;
          }
          else {
            if (prediction > 0) custom_pht[custom_pht_sel][custom_pht_index]--;
          }

          custom_ghr = custom_ghr << 1 | outcome;
          local_bht[custom_pc_bht_index] = ((local_bht[custom_pc_bht_index] << 1) | outcome ) & custom_local_hist_mask;
           

      default:
        break;
    }
}
