library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity address is

	port(
		cntH			: in integer range 0 to 800;
		cntV			: in integer range 0 to 525;
		clk			: in std_logic;
		Led_gP1		: in std_logic;
		address		: out std_logic_vector (11 downto 0)
	);

end entity;

architecture rtl of address is
	signal cnt : integer range 0 to 2640 := 0;
	
	constant X0 : integer := 200 ; -- 56 + 144borde negro
	constant Y0 : integer := 55;  -- 20 + 35bordenegro
	constant X1 : integer := 234;
	constant X2 : integer := 268;
	constant X3 : integer := 638;
	constant X4 : integer := 671;
	constant X5 : integer := 705;
	
	
	signal Tx, Cx, Wx, Lx  : integer := 0;
	constant Ty : integer := 202+35;
	constant Cy : integer := 195+35;
	constant Wy : integer := 263+35;
	constant Ly : integer := 265+35;

	
begin

		Tx <= 555 + 144 when Led_gP1 = '0' else 39 + 144;
		Cx <= 46 + 144 when Led_gP1 = '0' else 562 + 144;
		Wx <= 523 + 144 when Led_gP1 = '0' else 7 + 144;
		Lx <= 12 + 144 when Led_gP1 = '0' else 528 + 144;
		
	process(clk)
	begin
		if rising_edge(clk) then
			if (cntH >= X0-3 and cntH < X0-3 + 24 and cntV >= Y0 and cntV < Y0 + 22) then    -- el -3 es para sincronizar
				cnt <= (cntH-(X0-3))+(cntV-Y0)*24;
			elsif (cntH >= X1-3 and cntH < X1-3 + 24 and cntV >= Y0 and cntV < Y0 + 22) then
				cnt <= (cntH-(X1-3))+(cntV-Y0)*24;
			elsif (cntH >= X2-3 and cntH < X2-3 + 24 and cntV >= Y0 and cntV < Y0 + 22) then
				cnt <= (cntH-(X2-3))+(cntV-Y0)*24;
			elsif (cntH >= X3-3 and cntH < X3-3 + 24 and cntV >= Y0 and cntV < Y0 + 22) then
				cnt <= (cntH-(X3-3))+(cntV-Y0)*24;
			elsif (cntH >= X4-3 and cntH < X4-3 + 24 and cntV >= Y0 and cntV < Y0 + 22) then
				cnt <= (cntH-(X4-3))+(cntV-Y0)*24;
			elsif (cntH >= X5-3 and cntH < X5-3 + 24 and cntV >= Y0 and cntV < Y0 + 22) then
				cnt <= (cntH-(X5-3))+(cntV-Y0)*24;
				
			elsif (cntH >= Tx-3 and cntH < Tx-3 + 44 and cntV >= Ty and cntV < Ty + 60) then
				cnt <= (cntH-(Tx-3))+(cntV-Ty)*44;
			elsif (cntH >= Cx-3 and cntH < Cx-3 + 32 and cntV >= Cy and cntV < Cy + 69) then
				cnt <= (cntH-(Cx-3))+(cntV-Cy)*32;
			elsif (cntH >= Wx-3 and cntH < Wx-3 + 110 and cntV >= Wy and cntV < Wy + 13) then
				cnt <= (cntH-(Wx-3))+(cntV-Wy)*110;
			elsif (cntH >= Lx-3 and cntH < Lx-3 + 100 and cntV >= Ly and cntV < Ly + 20) then
				cnt <= (cntH-(Lx-3))+(cntV-Ly)*100;
			end if;
		end if;
	end process;
	address <= std_logic_vector(to_unsigned(cnt,12));
end rtl;