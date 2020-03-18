`timescale 1ns/1ps

module ram #(
    parameter ADDR_WIDTH = 8,
    parameter DATA_WIDTH = 8
    ) (
    input wire [ADDR_WIDTH - 1:0] addr,
    input wire [DATA_WIDTH - 1:0] data_in,
    input wire [DATA_WIDTH - 1:0] data_out,
    input wire cs, we, oe
);

    reg [DATA_WIDTH - 1:0] mem [0:(1 << ADDR_WIDTH) - 1];
    assign data_out = (cs && oe && !we) ? mem[addr] : {DATA_WIDTH{1'bz}};

    always @(cs or we or oe or addr) begin

        if (cs && we && !oe) begin
            mem[addr] <= data_in;
        end

    end


endmodule
