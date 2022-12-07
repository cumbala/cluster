/// @code
/// cluster.c
///
/// Author: Konstantin Romanets, xroman18@stud.fit.vut.cz
/// Date:   27.11.2022
/// Desc:   Implementation of cluster.c file for IZP Project 2
///         Additional functions: FreeClusters, IsFloat, IsNumber, DuplicateId, ParseObject
///
/// VUT FIT 2022
/// @endcode
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <string.h> // strtok, strspn, strcspn

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);

    // Set values
    c->size = 0;
    c->capacity = cap;

    // If cap is not zero - alloc, otherwise it's going to be null anyway
    if (cap != 0)
        c->obj = malloc(cap * sizeof(struct obj_t));
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    assert(c != NULL);

    // Free obj
    free(c->obj);
    // Init as empty
    c->size = 0;
    c->capacity = 0;
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    assert(c != NULL);

    // Only resize, when needed
    if (c->size >= c->capacity)
        c = resize_cluster(c, c->capacity + CLUSTER_CHUNK);

    // Add to the last free position and increment size
    c->obj[c->size] = obj;
    c->size++;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);
    
    for (int i = 0; i < c2->size; i++)
        append_cluster(c1, c2->obj[i]);

}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);
    
    // Clear memory
    clear_cluster(&carr[idx]);

    // Move everything to the left
    for (int i = idx; i < narr - 1; i++)
        carr[i] = carr[i + 1];

    return narr - 1;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);
    
    return sqrtf(powf(o1->x - o2->x, 2.0f) + powf(o1->y - o2->y, 2.0f));
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    // Set this to some big value
    float min = INFINITY;

    // Find the smallest distance
    for (int i = 0; i < c1->size; i++)
        for (int j = 0; j < c2->size; j++) {
            float dist = obj_distance(&c1->obj[i], &c2->obj[j]);
            if (dist < min)
                min = dist;
        }

    return min;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    // Set this to some big value
    float min = INFINITY;

    // Precaution: if we never find the smallest distance
    // then we are returning the first two clusters
    // 
    // Just like variable initialization
    *c1 = 0;
    *c2 = 1;

    // Find the smallest distance
    for (int i = 0; i < narr; i++)
        for (int j = i + 1; j < narr; j++) {
            float dist = cluster_distance(&carr[i], &carr[j]);
            if (dist < min) {
                min = dist;
                *c1 = i;
                *c2 = j;
            }
        }
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/// Frees all objects and clusters
/// @param clusters Array of clusters
/// @param n Number of clusters
void FreeClusters(struct cluster_t *clusters, int n) {
    // Free all objs in clusters
    for (int i = 0; i < n; i++) {
        clear_cluster(&clusters[i]);
    }
    // And finally - the array itself
    free(clusters);
}

/// Checks whether the given string is a @c float number
/// @remarks A weird way to check for float, but it works
/// If the string is @c "2.145" then convering it to @c int will result in @c 2
/// and converting it @c float will result in @c 2.145
/// Comparing the two with typecast will yield the result
/// @param str The string to check
/// @return 1 if the string is a float number, 0 otherwise
int IsFloat(const char *str) {
    float f = atof(str);
    int i = atoi(str);
    return (float)i != f;
}

/// Checks whether the string is a valid number
/// @remarks Works by checking whether the string contains allowed characters
/// @param str String to check
/// @return 1 if the string is a valid number, 0 otherwise
int IsNumber(const char *str) {
    return strspn(str, "0123456789") == strlen(str);
}

/// Searches the clusters array for duplicates
/// @remarks Will work only in the input sequence,
/// because only the first point of @c cluster_t->obj is checked
/// @param clusters The clusters array
/// @param n The number of clusters
/// @return The index of the first duplicate, or -1 if there are no duplicates
int DuplicateId(struct cluster_t *clusters, int n) {
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (clusters[i].obj[0].id == clusters[j].obj[0].id)
                return clusters[i].obj[0].id;

    return -1;
}

/// Parses one line of input
/// @param line Line to parse
/// @param id Parsed ID
/// @param x Parsed X coordinate
/// @param y Parsed Y coordinate
/// @return 0 on success, 1 on failure
int ParseObject(char *str, int *id, float *x, float *y) {
    // Remove linebreaks
    str[strcspn(str, "\r\n")] = 0;

    // Parse id
    char *id_str = strtok(str, " ");
    if (id_str == NULL || IsFloat(id_str) || !IsNumber(id_str))
        return 1;
    *id = atoi(id_str);

    // Parse X
    char *x_str = strtok(NULL, " ");
    if (x_str == NULL || !IsNumber(x_str))
        return 1;
    *x = atof(x_str);

    // Parse Y
    char *y_str = strtok(NULL, " ");
    if (y_str == NULL || !IsNumber(y_str))
        return 1;
    *y = atof(y_str);

    // If there is more than 3 tokens, return error
    char *extra = strtok(NULL, " ");
    if (extra != NULL)
        return 1;

    return 0;
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    // Open file
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: File could not be opened.\n");
        return -1;
    }

    // Read number of initial points, size can't be less than 1
    int size;
    if (fscanf(file, "count=%d\n", &size) != 1 || size < 1) {
        fprintf(stderr, "Error: Invalid file format.\n");
        fclose(file);
        return -1;
    }

    // Allocate memory for [size] clusters
    *arr = malloc(size * sizeof(struct cluster_t));
    if (*arr == NULL) {
        fprintf(stderr, "Error: Memory could not be allocated.\n");
        fclose(file);
        return -1;
    }

    // Add points
    // In the beginning, we have [size] clusters with 1 point each
    for (int i = 0; i < size; i++) {
        // Temp variables
        int id;
        float x, y;
        char line[100];
        // Read line from file
        fgets(line, 100, file);

        // Parse it and return if line is not valid
        if (ParseObject(line, &id, &x, &y) || (x < 0 || x > 1000) || (y < 0 || y > 1000)) {
            fprintf(stderr, "Error: Invalid file format.\nLine: %s.\n", line);
            FreeClusters(*arr, i);
            fclose(file);
            return -1;
        }

        // Initialize cluster
        init_cluster(&(*arr)[i], 1);
        (*arr)[i].obj[0].id = id;
        (*arr)[i].obj[0].x = x;
        (*arr)[i].obj[0].y = y;

        // Indicate, that we have 1 point there
        (*arr)[i].size = 1;
    }

    // Check for duplicates
    int dupl_id = DuplicateId(*arr, size);
    if(dupl_id != -1) {
        fprintf(stderr, "Error: Duplicate IDs: %d.\n", dupl_id);
        FreeClusters(*arr, size);
        fclose(file);
        return -1;
    }

    fclose(file);
    return size;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
    // Clusters array
    struct cluster_t *clusters = NULL;

    // Invalid number of args
    if (argc > 3 || argc < 2) {
        fprintf(stderr, "Error: Invalid number of arguments.\nUsage: ./cluster <filename> [<size>]\n");
        return 1;
    }

    // Clusters loading
    int size = load_clusters(argv[1], &clusters);
    if (size == -1) {
        return 1;
    }

    // Second argument
    int n = 1;
    if (argc == 3) {
        n = atoi(argv[2]);

        // If it is a float or the number is more than we have clusters read - error
        if (IsFloat(argv[2]) || n < 1 || n > size) {
            fprintf(stderr, "Error: Invalid number of clusters. Read %d, got %d.\n", size, n);
            FreeClusters(clusters, size);
            return 1;
        }
    }

    // Do the thing
    while (size > n) {
        int c1, c2;
        find_neighbours(clusters, size, &c1, &c2);
        merge_clusters(&clusters[c1], &clusters[c2]);
        sort_cluster(&clusters[c1]);
        remove_cluster(clusters, size, c2);
        size--;
    }

    // Print clusters
    print_clusters(clusters, size);

    // Free memory
    FreeClusters(clusters, n);
    return 0;
}
