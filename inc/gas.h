/*
 * gas.h
 *
 *  Created on: 04 апр. 2014 г.
 *      Author: kisame
 */

#ifndef GAS_H_
#define GAS_H_

class Gas {
private:
	double m_dMass;
public:
	Gas(double mass);
	virtual ~Gas();

	double getMass() const { return m_dMass; }
};

#endif /* GAS_H_ */
