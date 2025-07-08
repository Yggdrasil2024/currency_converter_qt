#include "converter.h"

double convert_to(double amount_to_convert, double source_rate, double target_rate) {
    //on ramene la devise en valeur de base(ex: USD)
    double amount_to_base = amount_to_convert / source_rate;
    //on convertis la devise dans la monnaie cible
    double amount_to_target = amount_to_base * target_rate;
    return amount_to_target;
}
