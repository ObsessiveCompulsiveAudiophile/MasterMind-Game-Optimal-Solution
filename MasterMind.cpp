#include <iostream>
#include <array>
#include <Windows.h> //just for timing
#include <ppl.h> //Concurrency
#define uFast uint_fast64_t //fastest variable type on 64 bit PCs

// Optimal Code Breaker
// by Serkan Gur 2022

// n,c can be changed for any combination of N(n,c)

const uFast n = 4;              // n repeatable digits (2-5)
const uFast c = 6;              // of c colors (2-7)
const uFast maxdepth = 7;       // max number of levels expectemaxdepth + 1 [7 for (4,6) , 5 for (4,4)], may be c+1

template <uFast A, uFast B> struct get_power {
    static const uFast value = A * get_power<A, B - 1>::value;
};
template <uFast A> struct get_power<A, 0> {
    static const uFast value = 1;
};
const uFast s = get_power<c, n>::value; // number of different combinations = pow(c,n)
const uFast imark0 = (uFast)(double(n) * ((double(n) / 2.0) + 1.5)); // total number of possible signs = n(n/2+1.5)
static uFast Valids[s + 1];

// function to recursively populate the whole set!
void FillSet(uFast c0, uFast n0, uFast& i0) { uFast i; if (n0 == 0) Valids[i0] = c0; else { for (i = 1; i <= c; i++) { FillSet(c0 * 10 + i, n0 - 1, i0); ++i0; } --i0; } }

int main() {
    static uFast iMark[imark0 + 1] = { 0 }; // indices of all possible plus minus combinations 
    uFast i, j, k, l, m, p, iv, iv2, o, o2, i00 = 1;
    uFast MapConsistent[maxdepth + 1][s + 1][imark0 + 1]; // tracks number of remaining consistents for each guess and sign at every level
    typedef uFast(iFiltered_)[s + 1]; // "typedef" used to store variables in adjacent memory addresses for speed (not necessary in some compilers)
    iFiltered_* iFiltered = new iFiltered_[maxdepth + 1]; // Full Set to cross by remaining consistents filtered by equivalance then most parts strength then pruned down from the weakest
    typedef uFast(iValid_)[s + 1]; // Consistent next guesses - i throughout the code stands for the indice rather than the actual number itself
    iValid_* iValid = new iValid_[maxdepth + 1];
    static uFast Mark[s + 1][s + 1]; // sign score of guess1 vs guess2
    uFast RowSum[1'000'000] = { 0 }; // large arrays and not on the Heap (for speed), requires "Stack size" increase
    typedef uFast(list_)[1'000'000]; // integer array holding solution path
    list_* list = new list_[maxdepth + 1];
    uFast ubPure[maxdepth + 1] = { 0 }, g[maxdepth + 1] = { 0 }, r[maxdepth + 1] = { 0 }, GuessSum[s + 1] = { 0 }; // upper bound of next set of consistents
    uFast a[n + 1] = { 0 }, b[n + 1] = { 0 };
    uFast guess, sign, lvl = 1, x = 0, x0 = 0, gMin = 0;
    std::array<uFast, s + 1> MPScore;
    static std::array<uFast, imark0 + 1> mult;
    for (i = 0; i < imark0; i++) mult[i + 1] = uFast(round(pow(16.0, double(i)))); k = 0;
    for (i = 0; i < n; i++) for (j = 0; j <= (n - i); j++) { k++; iMark[k] = i * 10 + j; } // Get all possible signs set
    iMark[0] = k; // first indice to keep total number of signs for no obvious reason
    iMark[imark0] = n * 10; // last indice is sign of code FOUND

    FillSet(0, n, i00); // get all possible numbers set

    for (i = 1; i <= s; i++) { iValid[1][i] = i; iFiltered[1][i] = i; }
    for (i = 1; i < s; i++) { // score them with each other
        for (j = i + 1; j <= s; j++) {
            a[0] = Valids[i]; b[0] = Valids[j]; p = 0; m = 0;
            for (k = 1; k < n; k++) { a[k] = a[0] % 10; b[k] = b[0] % 10; a[0] = (a[0] - a[k]) / 10; b[0] = (b[0] - b[k]) / 10; }
            a[n] = a[0]; b[n] = b[0];
            for (k = 1; k <= n; k++) if (a[k] == b[k]) { p++; a[k] = 0; b[k] = 9; }
            for (k = 1; k <= n; k++) { for (l = 1; l <= n; l++) if (a[k] == b[l]) { m++; b[l] = 9; break; } }
            Mark[i][j] = -0.5 * p * p + (n + 1.5) * p + m + 1; // give them proper sign indices
            Mark[j][i] = Mark[i][j];  // abuse symmetry
        }
        Mark[i][i] = imark0;
    }
    Mark[i][i] = imark0; // abuse itselfInputs: ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::array<uFast, 12> userinput;
    std::array<uFast, 12> gg;
    std::array<uFast, 12> ss;
    uFast count = 1;
    m = 0;
    std::cout << "\n";

onemore:

    std::cout << "Enter " << count << ". known guess(gggg) AND sign(plusminus) in the format 'ggggpm' " "OR '0' to create optimal solution table: ";
    std::cin >> userinput[count];
    if (userinput[count] > 0) {
        gg[count] = floor(userinput[count] / 100); ss[count] = userinput[count] - gg[count] * 100;
        for (i = 1; i <= s; i++) if (Valids[i] == gg[count]) { gg[count] = i; break; }
        for (i = 1; i <= imark0; i++) if (iMark[i] == ss[count]) { ss[count] = i; break; }
        count++;
        goto onemore;
    }

    uFast kk = s;
    for (j = 1; j < count; j++) {
        std::cout << j << ". " << userinput[j] << "\n";
        for (i = 1; i <= kk; i++) {
            if (Mark[iValid[1][i]][gg[j]] == ss[j]) {
                m++; iValid[1][m] = iValid[1][i];
            }
        }
        kk = m; m = 0;
    }

    if (kk == 0) {
        std::cout << "No solution with that, please try again!\n\n";
        for (i = 1; i <= s; i++) iValid[1][i] = i;
        goto onemore;
    }
    std::cout << "\n" << kk << " possible solutions!\n\n"
        << "Calculating...\n\n";
    if (kk == 1) { RowSum[1] = 1; list[1][1] = Valids[iValid[1][1]] * 100 + 40; goto finished; }

    uFast tim = GetTickCount64(); // start timer

    MapConsistent[0][0][0] = kk; for (i = 1; i <= maxdepth; i++) { g[i] = 1; r[i] = 1; } // initialization

    // Start///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

GetNext: ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // std::fill(&MapConsistent[lvl][0], &MapConsistent[lvl][0] + sizeof(uFast) * (imark0 + 1) * (s + 1), 0);
    // for (i = 1; i <= s; i++) { for (j = 1; j <= imark0; j++) MapConsistent[lvl][i][j] = 0;}
    memset(MapConsistent[lvl], 0, sizeof(uFast) * (imark0 + 1) * (s + 1)); //Clears faster
    uFast q[s + 1] = { 0 };
    k = lvl - 1;
    l = MapConsistent[k][g[k]][r[k]];

    iv = iValid[lvl][1];
    iv2 = iValid[lvl][2];
    for (i = 1; i <= s; i++) {
        o = Mark[i][iv];
        o2 = Mark[i][iv2];
        MapConsistent[lvl][i][o]++; // just helping SIMD perform
        MapConsistent[lvl][i][o2]++;
        q[i] += ((o * mult[o]) + (o2 * mult[o2]));
    }

    for (j = 3; j <= l; j++) { // still only helping SIMD
        iv = iValid[lvl][j];
        for (i = 1; i <= s; i++) {
            o = Mark[i][iv];
            MapConsistent[lvl][i][o]++; // populate number of remaining consistents for each sign
            q[i] += (o * mult[o]);

        }
    }

    k = 0; // Determine equivalance

    for (i = 1; i <= s; i++) {
        k++;
        iFiltered[lvl][k] = i;
        q[k] = q[i];
        for (m = 1; m < k; m++) { // Erase duplicates and find unique next guesses
            if (q[m] == q[k]) {
                k--;
                break;
            }
        }
    }

    // Pruning

    for (i = 1; i <= k; i++) {
        MPScore[i] = 0;
        for (j = 1; j <= imark0; j++) {                          // align upper bounds for new unique set
            MapConsistent[lvl][i][j] = MapConsistent[lvl][iFiltered[lvl][i]][j];
            MPScore[i] += MapConsistent[lvl][i][j] != 0;          // & apply MaxParts heuristics simultaneously
        }
        q[i] = MPScore[i];
    }

    concurrency::parallel_buffered_sort(&MPScore[1], &MPScore[k + 1]); // sort by MaxParts strength (only parallel part of the code - 
    // everywhere else serial code was faster due to initiation cost of max 1296 loops)

// Evaluate only top 12% or first 16 of the candidates sorted according to MP strength 

// l = (k > 119) ? k - 8 : 0.88 * k; // Magic accelerator ( x3 ), good enough for producing optimal tree, also helps with array sizes
    l = (k > 133) ? k - 16 : 0.88 * k;   // A bit slower but optimizes the worst of the worst game trees with full precision

    m = 0;
    for (i = 1; i <= k; i++) {
        if (q[i] >= MPScore[l]) {
            m++;
            iFiltered[lvl][m] = iFiltered[lvl][i];
            // memcpy(&MapConsistent[lvl][m][1], &MapConsistent[lvl][i][1], sizeof(uFast) * imark0); //this is faster with some compilers
            for (j = 1; j <= imark0; j++)
                MapConsistent[lvl][m][j] = MapConsistent[lvl][i][j];
        }
    }

    ubPure[lvl] = m; // new upperbound for pruned set of next guesses

    while (lvl)

    {

        for (guess = g[lvl]; guess <= ubPure[lvl]; guess++) // starting tree search

        {

            for (sign = r[lvl]; sign <= imark0; sign++)

            {
                if (MapConsistent[lvl][guess][sign] == 0) continue; // skip empty signs

                g[lvl] = guess;
                r[lvl] = sign;

                if (sign == imark0) { // Save if found
                    x++;
                    RowSum[x] = lvl;
                    for (i = 1; i <= lvl; i++) {
                        list[i][x] = Valids[iFiltered[i][g[i]]] * 100 + iMark[r[i]];
                    }
                    break;
                }

                j = 0; l = lvl - 1; k = lvl + 1;

                for (i = 1; i <= MapConsistent[l][g[l]][r[l]]; i++) // determine next set of Valids
                    if (Mark[iValid[lvl][i]][iFiltered[lvl][guess]] == sign) {
                        j++;
                        iValid[k][j] = iValid[lvl][i];
                    }

                lvl++; // forward one level

                if (lvl == maxdepth) { // skip if already too deep
                    for (i = 1; i <= MapConsistent[maxdepth - 1][g[maxdepth - 1]][r[maxdepth - 1]]; i++)
                        RowSum[x + i] = maxdepth;
                    x += (i - 1);
                    break;
                }

                if (j == 1) { // save path if only 1 consistent left and exit loop
                    ubPure[lvl] = 1;
                    iFiltered[lvl][1] = iValid[lvl][1];
                    MapConsistent[lvl][1][imark0] = 1;
                    r[lvl] = imark0;
                    x++;
                    RowSum[x] = lvl;
                    for (i = 1; i <= lvl; i++)
                        list[i][x] = Valids[iFiltered[i][g[i]]] * 100 + iMark[r[i]];
                    break;
                }

                goto GetNext; // If already here then cursed to RECURSE!

            }

            if ((g[lvl] != 1) && (ubPure[lvl] == g[lvl])) // is time to count steps?

            {
                gMin = 32768;
                l = lvl - 1;
                p = MapConsistent[l][g[l]][r[l]];
                x0 = x - (ubPure[lvl] * p);
                for (i = 1; i <= ubPure[lvl]; i++) {
                    GuessSum[i] = 0;
                    for (j = 1; j <= p; j++)
                        GuessSum[i] += RowSum[x0 + j + ((i - 1) * p)];
                    if (GuessSum[i] < gMin) {
                        gMin = GuessSum[i];
                        k = i;
                    }
                }

                for (i = 1; i <= p; i++) {
                    l = x0 + i;
                    m = l + ((k - 1) * p);
                    RowSum[l] = RowSum[m];
                    for (j = 1; j < maxdepth; j++)
                        list[j][l] = list[j][m];
                }

                x = x0 + p;
            }

            r[lvl] = 1;

        }

        g[lvl] = 1; ////////////////////////////////////////////////////////
        r[lvl] = 1;
        lvl--; // back propogate
        r[lvl]++; // next sign of previous level

    }
    // FINISHED!/////////////////////////////////////////////////////////////////////////////////////////FINISHED!

finished:

    j = 0; for (i = 1; i <= kk; i++) j += RowSum[i]; // Print look up table
    for (i = 1; i <= kk; i++) { for (k = 1; k <= RowSum[i]; k++) { std::cout << list[k][i] << "\t"; } std::cout << ":" << RowSum[i] << "\n"; }
    std::cout << j << " / " << kk << " = " << (double)j / kk << " average moves to finish!\n" << double(GetTickCount64() - tim) / 1000 << " seconds " << "\n\a\a\a";

    system("pause");

    return 0;

}
