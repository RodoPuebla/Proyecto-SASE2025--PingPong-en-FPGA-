library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity reloj is
    port (
      clk         : in std_logic; 
      reset       : in std_logic;
		ganador		: in std_logic;
		ganador2		: in std_logic;
      min       	: out integer range 0 to 9; 
      seg_u       : out integer range 0 to 9; 
      seg_d       : out integer range 0 to 9;
		enable_60hz	: out std_logic
    );
end entity;

architecture rtl of reloj is
    signal cnt    : integer range 0 to 24999999 := 0;
    signal pulse_1s  : std_logic := '0';

    signal segundos_u : integer range 0 to 9 := 0;
    signal segundos_d : integer range 0 to 5 := 0;
    signal minutos : integer range 0 to 9 := 0;
begin

    process(clk)
    begin
        if rising_edge(clk) then
            if cnt = 24_999_999 then
                cnt   <= 0;
                pulse_1s <= '1';
            else
                cnt   <= cnt + 1;
                pulse_1s <= '0';
            end if;
				
				 if cnt mod 416_666 = 0 then
                    enable_60hz <= '1';
             else
                    enable_60hz <= '0';
             end if;
        end if;
    end process;

    -- hasta 09:59
    process(clk,reset,ganador,ganador2)
    begin
		if reset = '0' or ganador = '1' or ganador2 = '1' then
                segundos_u <= 0;
                segundos_d <= 0;
                minutos  <= 0;
		elsif rising_edge(clk) then
			if pulse_1s = '1' then
				 if segundos_u = 9 then
					  segundos_u <= 0;
					  if segundos_d = 5 then
							segundos_d <= 0;
							if minutos = 9 then
								 minutos <= 0; 
							else
								 minutos <= minutos + 1;
							end if;
					  else
							segundos_d <= segundos_d + 1;
					  end if;
				 else
					  segundos_u <= segundos_u + 1;
				 end if;
			end if;
		end if;
    end process;
    min <= minutos;
    seg_d <= segundos_d;
    seg_u <= segundos_u;
end architecture;