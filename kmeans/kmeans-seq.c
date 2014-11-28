/* Sequential version of kmeans of n-d points*/
/* TODO better way to manage malloc/free, fopen/fclose */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

FILE *fin;
FILE *fout;

int num_dimensions;
int num_points;
int num_clusters;

/* input points */
double **points;

/* centroids */
double **centroids[2];
int c_idx;

/* number of children in each cluster */
int *num_children;

/* affinity */
int *affinity;

/* parse cli arguments */
void parse_args(int argc, char *argv[]);

/* print error msg and exit */
void error_exit(char *msg);

/* read data from input file */
void get_data();

/* output centroids */
void output_result();

/* euclidean distance */
double distance(double *point1, double *point2);

/* kmeans */
void kmeans();

/* Usage: ./kmeans-seq <input-file> <output-file> <#dimension> <#points> <#cluster> */
int main(int argc, char *argv[])
{
    parse_args(argc, argv);

    get_data();

    kmeans();

    output_result();

    for (int i = 0 ; i < num_points; i++)
        free(points[i]);
    free(points);
    for (int i = 0 ; i < num_clusters; i++) {
        free(centroids[0][i]);
        free(centroids[1][i]);
    }
    free(num_children);
    free(affinity);
}

void parse_args(int argc, char *argv[])
{
    if (argc != 6)
        error_exit("wrong number of args\n");

    if ((fin = fopen(argv[1], "r")) == NULL)
        error_exit("wrong number of args\n");

    if ((fout = fopen(argv[2], "w")) == NULL)
        error_exit("wrong number of args\n");

    if ((num_dimensions = atoi(argv[3])) <= 0)
        error_exit("wrong number of dimensions\n");

    if ((num_points = atoi(argv[4])) <= 0)
        error_exit("wrong number of points\n");

    if ((num_clusters = atoi(argv[5])) <= 0)
        error_exit("wrong number of clusters\n");
	num_points *= num_clusters;

    points = (double **)malloc(sizeof(double *) * num_points);
    for (int i = 0 ; i < num_points; i++)
        points[i] = (double *)malloc(sizeof(double) * num_dimensions);
    centroids[0] = (double **)malloc(sizeof(double) * num_dimensions * num_clusters);
    centroids[1] = (double **)malloc(sizeof(double) * num_dimensions * num_clusters);
    for (int i = 0 ; i < num_clusters; i++) {
        centroids[0][i] = (double *)malloc(sizeof(double) * num_clusters);
        centroids[1][i] = (double *)malloc(sizeof(double) * num_clusters);
    }
    c_idx = 0;
    num_children = (int *)malloc(sizeof(int) * num_clusters);
    affinity = (int *)malloc(sizeof(int) * num_points);
}

void error_exit(char *msg)
{
    printf("%s", msg);
    exit(1);
}

void get_data()
{
    printf("reading data...\n");
    for (int i = 0; i < num_points; i++) {
        for (int j = 0; j < num_dimensions; j++) {
            fscanf(fin, "%lf", &points[i][j]);
            if (j < num_dimensions-1)
                fscanf(fin, ",");
        }
        fscanf(fin, "\n");
    }

    fclose(fin);
    printf("reading data finished\n");
}

void output_result()
{
    for (int i = 0; i < num_clusters; i++) {
        for (int j = 0; j < num_dimensions; j++) {
            fprintf(fout, "%lf", centroids[c_idx][i][j]);
            if (j < num_dimensions-1)
                fprintf(fout, ",");
        }
        fprintf(fout, "\n");
    }

    fclose(fout);
}

double distance(double *point1, double *point2)
{
    double result = 0;
    for (int i = 0; i < num_dimensions; i++)
        result += (point1[i] - point2[i]) * (point1[i] - point2[i]);

    return sqrt(result);
}

void kmeans()
{
    printf("kmeans start...\n");
    // pick first k points as centroids
    // TODO use better init algorithm
    for (int i = 0; i < num_clusters; i++)
        for (int j = 0; j < num_dimensions; j++)
            centroids[c_idx][i][j] = points[i][j];

    double change = 1;
    int round = 0;

    while (change > 0.1) {
        printf("round %d\n", ++round);
        
        // compute affinity
        for (int i = 0; i < num_points; i++) {
            int best_centroid = 0;
            double min_distance = distance(points[i], centroids[c_idx][0]);
            for (int j = 1; j < num_clusters; j++) {
                double this_distance = distance(points[i], centroids[c_idx][j]);
                if (this_distance < min_distance) {
                    best_centroid = j;
                    min_distance = this_distance;
                }
            }
            affinity[i] = best_centroid;
        }

        // compute new centroids
        c_idx = 1 - c_idx;
        for (int i = 0; i < num_clusters; i++) {
            num_children[i] = 0;
            for (int j = 0; j < num_dimensions; j++)
                centroids[c_idx][i][j] = 0;
        }

        for (int i = 0; i < num_points; i++) {
            num_children[affinity[i]]++;
            for (int j = 0; j < num_dimensions; j++)
                centroids[c_idx][affinity[i]][j] += points[i][j];
        }

        for (int i = 0; i < num_clusters; i++)
            for (int j = 0; j < num_dimensions; j++)
                centroids[c_idx][i][j] /= num_children[i];

        // compute change
        change = 0;
        for (int i = 0; i < num_clusters; i++)
            change += distance(centroids[c_idx][i], centroids[1-c_idx][i]);
    }
    printf("kmeans finished...\n");
}
