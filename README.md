PMPL
====

V této složce (`./`) se nacházejí řešená cvičení. Každému cvičení je přiřazen jeden soubor `.cpp`
s definicí `main`. Kompilací a během těchto lze vygenerovat výsledky do složky `./data`. Neexistuje-li tato složka, je
ji třeba manuálně vytvořit. Data nejsou součástí repozitáře z důvodu velikosti.

Pro vizualizaci výsledků jsou napsané skripty ve složce `./scripts`. Pro jejich správný
běh je zapotřebí Python 3 a data ze složky `./data`. Obrázky a videa jsou
generovány do složky `./images`. Referenční výsledky jsou součástí repozitáře.

Komentáře ke kódu
-----------------
Kód je navržen tak aby usnadňoval orientaci a pochopení. V reálném případě bych neprováděl
deklarace v `.cpp` souborech a obecně sdílel více logiky. 

Kód je okomentován přímo v souborech. Komntáře jsou u funkcí, kterým je třeba věnovat pozornost
pro pochopení funkcionality. Neokomentované funkce by měli mít zřejmý účel, k jehož pochopení
postačují jméno a parametry.

Krátký přehled výsledků
-----------------------
Pojmenování jednotlivých úloh je zde zavedeno pro snažší orientaci. Nevystihuje však celou podstatu
prováděných výpočtů a může tak být zavádějící. 

**Solární systém**

Metodu leap-frog jsou vypočítany trajektrie Země, Slunce a Měsíce. Dále je pak sledována celková
změna energie systému. Výsledky jsou na obrázcích `solar.png`, `solar_zoom-png` a `solar_energy.png`.
Simulace je provedena pro jeden rok. Je patrné, že energie systému se v pruběhu simulace mění,
ale má peridodický průběh. To v principu opraňuje simulovat i velké množství oběhů. 
Zajímavé je si povšimnout, že trajektorie Měsíce je dána především vlivem slunce, nikoliv Země.

**Metoda SOR pro výpočet potenciálu**

Metodou SOR je proveden výpočet potenciálu s dirichletovou okrajovou podmínkou danou funkcí
`exp(-2x)cos(2y)`. Výpočet je proveden bez zdrojů, jedná se tedy o řešení Laplaceovi rovnice.
Pro výpočet je zvolen optimální parametr `omega`, na základě několika simulací. Tato
optimalizace je shrnuta na `sor_optimization.png`. Výsledný potenciál je na `sor.png`. Pro
srovnání s metodou výpočtu pomocí Monte Carlo simulací je tento výpočet proveden na různě velkých
sítích a zaznamenána délka běhu simulace. Přesná absolutní délka běhu není relevantní,
je sledován brůběh a řádový odhad. Pro ověření správnosti implmentace byla v průběhu
vývoje sledována l1-norma rodílu n umerického a anylytického řešení.

**Porezní materiál**

Úloha na porézní materiál byla efektivně převedena na úlohu hledání cesty v grafu.
Závislost pravděpodobnosti existnce cesty `q` ve 2D diskrétní síti náhodně zaplněné s pravěpodobnostní
`p` na `p` je na vyobrazena na `porous.png`. Zde dochází k prudkému zlomu okolo `p = 0.4`.

**Monte Carlo**

Metoda Monte Catlo je použita pro výpočet potenciálu v jednom bodě prostoru. Je tak učiněno
pro konkrétní dirichletovu okrajovou podmínku shodou s metodou SOR. Aplikace na výpočet
potenciálu mezi elektrodami je pak triviální. Přesnost výsledku je vyhodnocena na základě
opakování metody činí v bodě (x, y) = (0.5, 0.5) 0.20 +/- 0.01. Prakticky se rozdíl od analytického řešení
v tomto bodě s pohybuje v řádu tisícin. Vyhodnocení opakování je shrnuta na obrázku `carlo_precision.png`.
Stejně jako pro SOR je tento výpočet proveden na různě velkých sítích a zaznamenána délka
běhu simulace. Na základě tohoto je pak uděláno normované srovnání s metodou SOR na
obrázku `carlo_sor_compare.png`. To ukazuje na fakt, že je potenciálně možné, že simulace
metodou Monte Carlo budou výhodnější pro velké sítě, kde chceme určit pouze lokální potenciál.


**Zdroj částic bez srážek**

Byl naprgramován homogení termální zdroj částic bez úvážování srážek. Bylo zaznamenáno rozdělení
velikostí rychlostí a to jak objemové na obrázku `collisionless_distribution.png` tak pro částice procházející
jednou stranou výpočetní oblasti `collisionless_side_distribution`. Pozorovaná kinetická energie
samozřejmě zůstala v průběhu simulace konstantní, viz. `collisionless_energy.png`.
Dale bylo ze 10000 částic vybráno 100 a jejich trajektorie byly vyobrazeny v krátké animaci
`collisionless_trajectories.mp4`.

**Zdroj částic s uvažováním srážek**

Do předchozí simulace byly doplněny srážky s pozadím za předpokladu studeného plazmatu.
Byla volena konstatní srážková frekvence (účinný průřez). Srážky způsobili změnu teploty, předaním energie
pozadí, jak je tomu patrné z `collisional_energy.png` a `collisional_distribution.png`.
Částicím zůstalo termální rozdělení, pouze s jinou teplotou.
Stejně jako v předchozím případě bylo ze 10000 částic vybráno 100 a jejich trajektorie byly vyobrazeny v krátké animaci
`collisional_trajectories.mp4`.

**Borisův algoritmus**

Byla naimplementována neralitivistikcá a i relaticistická verze borisova algoritmu. Následně byl sledován elektron
v konstatním magnetickém poli mířícím ve směru osy z a elektrickém pole ve směru osy x. Trajektorie
elektronu byla srovnána s nerelativistickým analytickým řešením úlohy na obrázku `boris.png` a to
pro dva různé časové kroky. Numerické řešení se se zkracujícím se časovým krokem blíží řešení analytickému.
Dále pak bylo srovnáno neraltivistické řešení s relativistickým a to v případě, že analytická
driftová byla blízká rychlosti světla na obrázku `boris_relativistic.png`. Zde je patrný jasný rozdíl
daný právě maximální driftovou rychlostí rovnou rychlosti světla. Pro ověření byla v relativistickém
případě sledována velikost rychlosti částice. Výsledek je na obrázku `boris_relativistic_speed.png`.