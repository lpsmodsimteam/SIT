`timescale 1ns / 1ps

/*
 * AXI4-Stream MT19937 Mersenne Twister
 */
module mt19937 #(
    parameter N = 624,  // degree of recurrence
    parameter M = 397,  // middle word
    parameter R = 31,  // separation point of one word
    parameter A = 32'h9908B0DF,  // coefficients of the rational normal form twist matrix
    parameter U = 11,  // shift size used in the tempering process
    parameter D = 32'hFFFFFFFF,  // XOR mask used in the tempering process
    parameter S = 7,  // shift size used in tempering process
    parameter B = 32'h9D2C5680,  // XOR mask used in the tempering process
    parameter T = 15,  // shift size used in tempering process
    parameter C = 32'hEFC60000,  // XOR mask used in the tempering process
    parameter L = 18,  // shift size used in tempering process
    parameter F = 1812433253  // initialization parameter
) (
    input wire clk,
    input wire rst,

    /*
     * AXI output
     */
    output wire [R:0] r_num,
    output wire       valid,
    input  wire       ready,

    /*
     * Status
     */
    output wire busy,

    /*
     * Configuration
     */
    input wire [R:0] seed_val,
    input wire       seed_start
);

  // state register
  localparam [1:0] STATE_IDLE = 2'd0, STATE_SEED = 2'd1;

  reg [1:0] state_reg = STATE_IDLE, state_next;

  reg [R:0] mt[N-1:0];
  reg [R:0] mt_save_reg = 0, mt_save_next;
  reg [9:0] mti_reg = N + 1, mti_next;

  reg [R:0] y1, y2, y3, y4, y5;

  reg [9:0] mt_wr_ptr;
  reg [R:0] mt_wr_data;
  reg mt_wr_en;

  reg [9:0] mt_rd_a_ptr_reg = 0, mt_rd_a_ptr_next;
  reg [R:0] mt_rd_a_data = 0;

  reg [9:0] mt_rd_b_ptr_reg = 0, mt_rd_b_ptr_next;
  reg [R:0] mt_rd_b_data = 0;

  reg [R:0] product_reg = 0, product_next;
  reg [R:0] factor1_reg = 0, factor1_next;
  reg [R:0] factor2_reg = 0, factor2_next;
  reg [4:0] mul_cnt_reg = 0, mul_cnt_next;

  reg [R:0] output_axis_tdata_reg = 0, output_axis_tdata_next;
  reg output_axis_tvalid_reg = 0, output_axis_tvalid_next;

  reg busy_reg = 0;

  assign r_num = output_axis_tdata_reg;
  assign valid = output_axis_tvalid_reg;

  assign busy  = busy_reg;

  always @(*) begin
    state_next = 2'bz;

    mt_save_next = mt_save_reg;
    mti_next = mti_reg;

    mt_wr_data = 0;
    mt_wr_ptr = 0;
    mt_wr_en = 0;

    y1 = 32'bz;
    y2 = 32'bz;
    y3 = 32'bz;
    y4 = 32'bz;
    y5 = 32'bz;

    mt_rd_a_ptr_next = mt_rd_a_ptr_reg;
    mt_rd_b_ptr_next = mt_rd_b_ptr_reg;

    product_next = product_reg;
    factor1_next = factor1_reg;
    factor2_next = factor2_reg;
    mul_cnt_next = mul_cnt_reg;

    output_axis_tdata_next = output_axis_tdata_reg;
    output_axis_tvalid_next = output_axis_tvalid_reg & ~ready;

    case (state_reg)
      STATE_IDLE: begin
        // idle state
        if (seed_start) begin
          mt_save_next = seed_val;
          product_next = 0;
          factor1_next = mt_save_next ^ (mt_save_next >> (R - 1));
          factor2_next = F;
          mul_cnt_next = R;
          mt_wr_data = mt_save_next;
          mt_wr_ptr = 0;
          mt_wr_en = 1;
          mti_next = 1;
          state_next = STATE_SEED;
        end else if (ready) begin
          if (mti_reg == N + 1) begin
            mt_save_next = 32'd5489;
            product_next = 0;
            factor1_next = mt_save_next ^ (mt_save_next >> (R - 1));
            factor2_next = F;
            mul_cnt_next = R;
            mt_wr_data = mt_save_next;
            mt_wr_ptr = 0;
            mt_wr_en = 1;
            mti_next = 1;
            state_next = STATE_SEED;
          end else begin
            if (mti_reg < N - 1) mti_next = mti_reg + 1;
            else mti_next = 0;

            if (mt_rd_a_ptr_reg < N - 1) mt_rd_a_ptr_next = mt_rd_a_ptr_reg + 1;
            else mt_rd_a_ptr_next = 0;

            if (mt_rd_b_ptr_reg < N - 1) mt_rd_b_ptr_next = mt_rd_b_ptr_reg + 1;
            else mt_rd_b_ptr_next = 0;

            mt_save_next = mt_rd_a_data;
            y1 = {mt_save_reg[R], mt_rd_a_data[R-1:0]};
            y2 = mt_rd_b_data ^ (y1 >> 1) ^ (y1[0] ? A : 32'h0);
            y3 = y2 ^ (y2 >> U);
            y4 = y3 ^ ((y3 << S) & B);
            y5 = y4 ^ ((y4 << T) & C);
            output_axis_tdata_next = y5 ^ (y5 >> L);
            output_axis_tvalid_next = 1;
            mt_wr_data = y2;
            mt_wr_ptr = mti_reg;
            mt_wr_en = 1;
            state_next = STATE_IDLE;
          end
        end else begin
          state_next = STATE_IDLE;
        end
      end
      STATE_SEED: begin
        if (mul_cnt_reg == 0) begin
          if (mti_reg < N) begin
            //mt_save_next = F * (mt_save_reg ^ (mt_save_reg >> R - 1)) + mti_reg;
            mt_save_next = product_reg + mti_reg;
            product_next = 0;
            factor1_next = mt_save_next ^ (mt_save_next >> (R - 1));
            factor2_next = F;
            mul_cnt_next = R;
            mt_wr_data = mt_save_next;
            mt_wr_ptr = mti_reg;
            mt_wr_en = 1;
            mti_next = mti_reg + 1;
            mt_rd_a_ptr_next = 0;
            state_next = STATE_SEED;
          end else begin
            mti_next = 0;
            mt_save_next = mt_rd_a_data;
            mt_rd_a_ptr_next = 1;
            mt_rd_b_ptr_next = M;
            state_next = STATE_IDLE;
          end
        end else begin
          mul_cnt_next = mul_cnt_reg - 1;
          factor1_next = factor1_reg << 1;
          factor2_next = factor2_reg >> 1;
          if (factor2_reg[0]) product_next = product_reg + factor1_reg;
          state_next = STATE_SEED;
        end
      end
    endcase
  end

  always @(posedge clk) begin
    if (rst) begin
      state_reg <= STATE_IDLE;
      mti_reg <= N + 1;
      mt_rd_a_ptr_reg <= 0;
      mt_rd_b_ptr_reg <= 0;
      product_reg <= 0;
      factor1_reg <= 0;
      factor2_reg <= 0;
      mul_cnt_reg <= 0;
      output_axis_tdata_reg <= 0;
      output_axis_tvalid_reg <= 0;
      busy_reg <= 0;
    end else begin
      state_reg <= state_next;

      mt_save_reg = mt_save_next;
      mti_reg <= mti_next;

      mt_rd_a_ptr_reg <= mt_rd_a_ptr_next;
      mt_rd_b_ptr_reg <= mt_rd_b_ptr_next;

      product_reg <= product_next;
      factor1_reg <= factor1_next;
      factor2_reg <= factor2_next;
      mul_cnt_reg <= mul_cnt_next;

      output_axis_tdata_reg <= output_axis_tdata_next;
      output_axis_tvalid_reg <= output_axis_tvalid_next;

      busy_reg <= state_next != STATE_IDLE;

      if (mt_wr_en) begin
        mt[mt_wr_ptr] <= mt_wr_data;
      end

      mt_rd_a_data <= mt[mt_rd_a_ptr_next];
      mt_rd_b_data <= mt[mt_rd_b_ptr_next];

    end
  end

endmodule
