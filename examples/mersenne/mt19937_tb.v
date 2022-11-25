
// Language: Verilog 2001

`timescale 1ns / 1ps

/*
 * Testbench mt19937
 */
module mt19937_tb;

  // Parameters

  localparam NUM_ITERS = 624;
  localparam NUM_BITS = 32;
  localparam ADDR_WIDTH = 8;

  // Inputs
  reg clk;
  reg rst;

  reg [NUM_BITS-1:0] seed_val;
  reg seed_start;
  reg ready;

  // Outputs
  wire [NUM_BITS-1:0] r_num;
  wire valid;
  wire busy;

  always #1 clk = ~clk;

  mt19937 mt (
      .clk(clk),
      .rst(rst),
      .r_num(r_num),
      .valid(valid),
      .ready(ready),
      // Configuration
      .seed_val(seed_val),
      .seed_start(seed_start),
      // Status
      .busy(busy)
  );

  reg [ADDR_WIDTH-1:0] address;
  reg [NUM_BITS-1:0] data_in;

  wire [NUM_BITS-1:0] data_out;

  reg cs;
  reg we;
  reg oe;

  ram mem (
      .clk(clk),
      .cs(cs),
      .we(we),
      .oe(oe),
      .address(address),
      .data_in(data_in),
      .data_out(data_out)
  );

  integer i;

  // Test stimulus
  initial begin

    $monitor("%5d rnum=%10d addr=%1d din=%10d dout=%10d", $time, r_num, address, data_in, data_out);

    clk <= 0;
    rst <= 0;

    seed_val <= 0;
    seed_start <= 0;
    ready <= 0;

    cs <= 0;
    we <= 0;
    oe <= 0;

    rst <= 1;
    repeat (1) @(posedge clk);
    rst <= 0;

    seed_start <= 1;
    repeat (1) @(posedge clk);
    seed_start <= 0;

    ready <= 1;

    repeat (NUM_ITERS * NUM_BITS) @(posedge clk);
    repeat (2) @(posedge clk);

    for (i = 0; i < 10; i = i + 1) begin
      cs <= 1;
      we <= 1;
      oe <= 0;
      address <= i;
      data_in <= r_num;
      repeat (1) @(posedge clk);
    end

    ready <= 0;

    for (i = 0; i < 10; i = i + 1) begin
      cs <= 1;
      we <= 0;
      oe <= 1;
      address <= i;
      repeat (1) @(posedge clk);
    end

    $finish;

  end

endmodule
