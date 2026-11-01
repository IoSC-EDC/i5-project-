const int THRESHOLD = 500; // Midpoint between line (~25) and white floor (~1020)
int v = constrain(map(raw, 25, 1020, 1000, 0), 0, 1000);
const int weight[5] = {0, 1000, 2000, 3000, 4000};
...
position = weighted / sum;
