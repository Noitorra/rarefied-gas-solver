#ifndef NORMALIZER_H_
#define NORMALIZER_H_

class Normalizer {
public:
	Normalizer();
	void loadFromFile();

private:
	double n; // density
	double t; // temperature
	double p; // real normilize operation
	double tau; // time
	double m; // mass
	double e; // speed cut
	double l; // the mean free path of a molecule
};

#endif // NORMALIZER_H_
