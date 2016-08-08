#ifndef NORMALIZER_H_
#define NORMALIZER_H_

class Normalizer {
public:

    enum class Type {
        TEMPERATURE,
        PRESSURE,
        FLOW,
        LAMBDA
    };

	Normalizer();
	void loadFromFile();

    double normalize(const double& value, const Type& type) const;
    void normalize(double& value, const Type& type) const;

    double restore(const double& value, const Type& type) const;
    void restore(double& value, const Type& type) const;
private:
	double m_dDensity; // density
	double m_dTemperature; // temperature
	double m_dPressure; // real normilize operation
	double m_dTime; // time
	double m_dMass; // mass
	double m_dVelocity; // speed cut
	double m_dLength; // the mean free path of a molecule
};

#endif // NORMALIZER_H_
