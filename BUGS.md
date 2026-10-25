WARNING this file is in written Polish, you have to translate it by yourself if you want
Or just learn Polish. Aint easy but i encurage. 
This language is awsome..  


1. Co popsu³eœ (historia)

W historii gita nie ma dzia³aj¹cej wersji. Najpowa¿niejsze b³êdy s¹ obecne ju¿ w pierwszym commicie 98115f9 (2020), a tak¿e w starej kopii stk_stasm.h.aw2576, która trafi³a do repo razem z nim. PóŸniejsze commity robi³y prawie wy³¹cznie zmiany nazw i kosmetykê:

4e7ba13: x64r8/x64r9 przemianowane na x64r08/x64r09, MAX_PARARELL z 32 na 64, dodane rax..rdi;
366c5dd, f22cc0c, e675c47: komentarze, | zamienione na ||, przyk³ad.

Wersja, która „dzia³a³a”, pochodzi sprzed gita, co zreszt¹ piszesz w README. Œlad: README i aw2576 u¿ywaj¹ nazw s_x86/m_x86, a obecny kod __stasm_x86.

2. Blokowanie (__TLOCK/__TUNLOCK, linie 1190–1195)

Twój niepokój jest uzasadniony. Zakleszczenie mo¿e siê zdarzyæ nawet bez rekurencji.

Operacja nie jest atomowa. Sprawdzenie max==0, odczyt i max-- to trzy osobne kroki, a ++max w unlocku te¿ nie jest atomowe. W teœcie (8 w¹tków, lock/unlock w pêtli) licznik po zakoñczeniu wynosi³ 63 zamiast 64. Przy MAX_PARARELL=2 wynosi³ 1 zamiast 2. Ka¿da zgubiona aktualizacja na sta³e zabiera jeden slot. Gdy licznik dojdzie do 0, ka¿dy kolejny __TLOCK krêci siê w nieskoñczonoœæ. To jest Twoje zakleszczenie.
Mo¿liwe przepe³nienie w dó³. Licznik ma typ uint8_t. Jeœli dwa w¹tki jednoczeœnie zobacz¹ max==1, oba zmniejsz¹ licznik i wyjdzie 255. Wtedy indeks wypada poza tablicê [65]. W teœcie tego nie zaobserwowa³em, ale nic tego nie blokuje.
WskaŸnik __stasm_x86 jest jeden, globalny, wspólny dla wszystkich w¹tków. Ka¿dy lock i unlock go nadpisuje, a blok asm czyta go póŸniej (mov esi, __stasm_x86). W¹tek A mo¿e wiêc pisaæ do slotu w¹tku B. W praktyce izolacji slotów nie ma w ogóle.
Unlock nie zwalnia „swojego” slotu, tylko podbija licznik (LIFO). Dzia³a to poprawnie tylko wtedy, gdy w¹tki zwalniaj¹ w odwrotnej kolejnoœci ni¿ blokowa³y.
Rekurencja g³êbsza ni¿ MAX oznacza, ¿e w¹tek czeka sam na siebie. To ju¿ opisa³eœ. Dochodz¹ do tego return i wyj¹tki miêdzy lock a unlock, które gubi¹ slot.
Brak volatile/atomic. Z moj¹ pust¹, inline’owan¹ zaœlepk¹ wait_us GCC przy -O2 usun¹³ pêtlê while ca³kowicie: pêtla bez efektów ubocznych jest w C++ UB. Z prawdziwym, nieprzezroczystym wait_us pewnie zadzia³a, ale nie mogê tego sprawdziæ bez stk_main.h.
__stasm() w ogóle nie wywo³uje __TLOCK. Komentarz w przyk³adzie („protected by atomic lock”) nie odpowiada kodowi. Do tego __TINIT nie da siê u¿yæ, z dwóch powodów:
__STASM_SHARE_STACK_CLEAR (l. 291–292) inkrementuje nieistniej¹ce _t++ i _c++ zamiast _thread++ i _i++;
brakuje terminatora __TINIT_code.

Uboczna uwaga: na x86-64 slot uint32_t × 8 ma 32 B, nie 64, wiêc dwa sloty dziel¹ jedn¹ liniê cache.

3. Struktury FPU (rozmiary zmierzone sizeof/offsetof)

FNSAVE: poprawne. FCW 0, FSW 4, FTW 8, FIP 12, FCS 16, FOP 18, FDP 20, FDS 24, ST0 28, krok 10 B. To siê zgadza z formatem 32-bit protected mode.

sizeof = 112, bo aligned(16) dope³nia strukturê; samo fsave zapisuje 108. To nie szkodzi.
Unia RIP/RDP w FNSAVE wprowadza w b³¹d. FSAVE nie ma formatu 64-bitowego, a RIP nak³ada siê na FIP+FCS+FOP.

FXSAVE: b³¹d krytyczny. Nag³ówek do offsetu 31 jest poprawny, ³¹cznie z uni¹ RIP/RDP dla fxsave64.

	jest	powinno byæ (Intel)
ST0 / ST1	32 / 42	32 / 48
XMM0	112	160
reserved	368	416
sizeof	464	512

Przyczyna: dla ST u¿y³eœ __STASM_STRUCT_STX_MMX (10 B) z FNSAVE, a w FXSAVE ka¿dy slot ma 16 B (10 B danych i 6 B zarezerwowane). Arytmetyka (464-416) pokazuje, ¿e zamierza³eœ 16 B. Skutek: fxsave zapisuje 48 B za ka¿dym elementem tablicy, czyli nadpisuje slot nastêpnego w¹tku, a ostatni element wychodzi poza tablicê. FTW jako uint16_t jest rozmiarowo OK, ale wa¿ny jest tylko m³odszy bajt (abridged tag).

XSAVE: niezgodne. Legacy dziedziczy b³¹d z FXSAVE (464 zamiast 512). Do tego:

PAD[64] to uint64_t, czyli 512 B. Nag³ówek ma 64 B. Pola XSTATE_BV, XCOMP_BV i RESERVED[6] w œrodku s¹ poprawne.
Twoje YMM0..15 (16 B ka¿dy) to w rzeczywistoœci tylko górne 128 bitów YMM. Rozmiar siê zgadza, nazwa nie.
Brakuje MPX (128 B) i opmask k0–k7 (64 B).
ZMMH ma 16×32 B, a Hi16_ZMM to ZMM16–31 w ca³oœci, czyli 16×64 B.

Typowe offsety formatu niekompaktowego: header 512, AVX 576, MPX 960/1024, opmask 1088, ZMM_Hi256 1152, Hi16_ZMM 1664, PKRU 2688. W³aœciwe wartoœci trzeba jednak czytaæ z CPUID.(EAX=0Dh,ECX=i), a rozmiar ca³oœci z CPUID.(0Dh,0).EBX. Przy AMX to ponad 11 KB.

OdpowiedŸ na TODO z commita 4e7ba13: rejestry s¹ czêœciami siebie nawzajem (XMM to dó³ YMM, a YMM to dó³ ZMM), ale XSAVE zapisuje je w kawa³kach, w ró¿nych miejscach. Unia tu nie pasuje.

Dodatkowo xsave/xrstor wymagaj¹ maski w EDX:EAX. Makra jej nie ustawiaj¹, wiêc maska jest przypadkowa i mo¿e zapisaæ wiêcej ni¿ 2304 B.

4. B³êdy w makrach (potwierdzone)
Ograniczenie „var dzia³a tylko raz” nie jest win¹ GCC. Preprocesor nie rozwija makra wewn¹trz jego w³asnego rozwiniêcia, wiêc drugie var (tak samo drugie save) zostaje w kodzie jako nierozwiniêty tekst __stasm_s_var(...). Dodatkowo:
nazwa zmiennej w kodzie nie jest zamieniana na %[a]; GAS widzi symbol a („ambiguous operand size”);
"0" jest wpisane na sztywno;
Clang odrzuca "+r" razem z "0".
save nie dzia³a w ogóle. __stasm_save_##what(__VA_ARGS__) daje "push eax\n"(mov eax,1;), bo makra save_* nie s¹ funkcyjne.
Kolejnoœæ push/pop przy odtwarzaniu:
__stasm_load_x86 i __stasm_load_x64 u¿ywaj¹ save_*, czyli pushuj¹ zamiast popowaæ;
load_x64rxx to pop r15 i potem piêæ push;
save_x64r8 nie pasuje do s³owa x64r08.
Problemy specyficzne dla x86-64:
push eax nie istnieje w trybie 64-bitowym;
push w inline asm nadpisuje red zone (128 B pod RSP) funkcji liœciowych;
offsety operandów wzglêdem RSP przestaj¹ siê zgadzaæ.
Trzy ró¿ne zestawy nazw obszarów zapisu, ¿aden siê nie ³¹czy:
asm u¿ywa _m_fsaved, _m_fxsaved, _m_axsaved;
.h deklaruje __stasm_fnsave, __stasm_fxsave, __stasm_xsave;
.cpp definiuje __stasm_fnsaved_stack i podobne.
Do tego obszary nie s¹ indeksowane per w¹tek. W .cpp (l. 98) etykieta "__stasm_axsaved_stack*" z gwiazdk¹ powoduje b³¹d asemblera przy -mavx.
-masm=intel. GCC nie definiuje __INLINE_INTEL__, wiêc po bloku zawsze wstawiane jest .att_syntax. Z -masm=intel kod wygenerowany przez kompilator po bloku przestaje siê asemblowaæ („operand size mismatch”). Bez tej opcji dzia³a.
Z³e nazwy rejestrów i makr:
clobbery "r8lb","r8hb","r8w","r8d" (i dla r9–r15) GCC odrzuca jako nieznane;
backticki `\ w l. 875–889 daj¹ „stray `”;
l. 847: __stasm_x64 = MAX zamiast _max;
l. 918: literówki x64rx_stack/x64xx_max.
AVX/AES: __stasm_r_avx(...) i __stasm_r_aes(...) nie maj¹ parametru mode, wiêc powstaje __stasm_r_mode (b³¹d przy -mavx). W blokach SSE i AVX jest emms, czyli instrukcja MMX.
Z³e nazwy makr kompilatora:
powinno byæ __SSE4_1__/__SSE4_2__, nie __SSE41__/__SSE42__;
powinno byæ __SSSE3__, nie __SSEE3__;
.cpp nazywa stos sse1, a .h sse, wiêc symbol __stasm_sse nie istnieje.
Przyk³ad variables.cpp:
#ifdef __i368__ to literówka i brakuje #endif, wiêc plik siê nie kompiluje;
testfunc nie ma return;
brakuje <cassert>.


Przeczyta³em threads/stk_threads.h (tam s¹ ATOMIC, ATOMIC_LOCK i ATOMIC_UNLOCK), time/stk_time.cpp (wait_us) i stk_MAIN.h. Makra skopiowa³em 1:1 do testu: 8 w¹tków po 200 tys. wejœæ do sekcji krytycznej, z licznikiem, ile w¹tków jest w niej naraz.

Jedno sprostowanie na pocz¹tek

User ma niepodzielne compare-and-exchange. To nie s¹ instrukcje uprzywilejowane, dzia³aj¹ w ring 3:

XCHG z pamiêci¹ ma niejawny LOCK (ju¿ na 386);
LOCK CMPXCHG i LOCK XADD s¹ od 486, LOCK CMPXCHG8B od Pentium;
dzia³a te¿ prefiks LOCK przy ADD/INC/DEC/OR/AND.

Po stronie kompilatora:

GCC/MinGW ma wbudowane __sync_val_compare_and_swap, __sync_fetch_and_add itd. (od GCC 4.1, bez C++11);
pod Windows s¹ InterlockedCompareExchange i InterlockedIncrement z WinAPI, wiêc dzia³aj¹ te¿ w BCB i MSVC;
w duchu stasm mo¿na po prostu napisaæ lock cmpxchg we wstawce.

Masz ju¿ nawet w cpu/stk_cpu.h funkcjê cpu_has_cmpxchg().

Dlaczego unikalny TSC nie chroni, nawet na jednym rdzeniu

rdtsc daje unikalny identyfikator, ale problem nie le¿y w unikalnoœci, tylko w oknie miêdzy sprawdzeniem a zapisem.

Przebieg na jednym rdzeniu:

W¹tek A wychodzi z while (__shared != 0), bo widzi 0. Planista wyw³aszcza go przed __shared = __unique.
W¹tek B przechodzi ca³y ATOMIC_LOCK: widzi 0, zapisuje swój TSC, mfence, odczyt zgadza siê z jego wartoœci¹. B jest w sekcji.
A wraca, zapisuje swój TSC (nadpisuje B), mfence, odczyt zgadza siê. A te¿ jest w sekcji.
Gdy B zrobi ATOMIC_UNLOCK, ustawi 0, chocia¿ A nadal jest w œrodku. Wtedy mo¿e wejœæ trzeci w¹tek.

mfence porz¹dkuje zapisy, ale nie skleja „sprawdŸ i zapisz” w jedn¹ niepodzieln¹ operacjê. Robi to dopiero XCHG/CMPXCHG, bo ca³a operacja jest jedn¹ instrukcj¹.

Wyniki testu (te makra, bez zmian)
	wszystkie rdzenie	jeden rdzeñ (taskset -c 0)
maks. w¹tków naraz w sekcji	2–3	2–3
wejœæ przy zajêtej sekcji	424 tys. – 513 tys.	56 tys. – 723 tys.
counter++ (oczekiwane 1 600 000)	1 511 135 – 1 551 480	1 600 000

Na jednym rdzeniu licznik wyszed³ poprawny tylko dlatego, ¿e GCC skompilowa³ counter++ do jednej instrukcji add [mem],1. Pojedyncza instrukcja nie zostanie przerwana w po³owie na tym samym rdzeniu. To jest prawdziwe Ÿród³o Twojej intuicji, ale dotyczy jednej instrukcji, a nie kilku linii kodu. Wykluczanie wzajemne i tak by³o z³amane: do 3 w¹tków naraz.

Inne rzeczy w tych makrach
Ka¿de wywo³anie ATOMIC(n) tworzy osobn¹ zmienn¹ static w danej funkcji. ATOMIC(1) w tsc_start() i ATOMIC(1) w tsc_init() to dwa ró¿ne locki i nie wykluczaj¹ siê nawzajem. Numer num niczego nie ³¹czy.
W buildzie 32-bitowym volatile int64_t jest zapisywany i czytany dwiema instrukcjami po 32 bity, wiêc inny w¹tek mo¿e zobaczyæ pó³ starej i pó³ nowej wartoœci.
Rekurencja w funkcji z ATOMIC_LOCK daje samozakleszczenie, bo zmienna jest wspólna dla wszystkich wywo³añ tej funkcji.
BARIER_ADD ma odwrócony warunek: while(__barier_locker==0). Przy wartoœci pocz¹tkowej 0 (stk_threads.cpp:11) czeka w nieskoñczonoœæ. Na szczêœcie makro nigdzie nie jest u¿ywane.
wait_us, ga³¹Ÿ nie-Windows, siê nie skompiluje:
1 000 000 ze spacjami;
% 100 000 zamiast miliona;
niedomkniêty nawias w while (nanosleep(&req,&rem);.
Pod Windows wait_us(0) i wait_us(1) daj¹ Sleep(0), czyli oddanie kwantu czasu, i to dzia³a.
Powi¹zanie ze stasm

__TLOCK/__TUNLOCK w stasm w ogóle nie u¿ywaj¹ ATOMIC_LOCK. Nie maj¹ nawet tej (dziurawej) ochrony, tylko go³e max--/++max na uint8_t. Opis w commicie stk („shared global (but protected atomic) variables”) odnosi siê wiêc do zamiaru, którego w kodzie stasm nie ma.