/*
 * ProductDebug.h
 *
 *  Created on: 14.06.2016
 *      Author: nid
 */

#ifndef PRODUCTDEBUG_H_
#define PRODUCTDEBUG_H_

#include "ConfigTypes.h"

namespace ProductDebug
{

class ProductDebug
{
public:
  ProductDebug() = default;
  virtual ~ProductDebug() = default;

  /**
   * @brief Setup CLI for setup and debuging
   *
   */
  void setupProdDebugEnv();

  /**
   * @brief Callback mechanism to allow saving the entered wifi credentials
   *
   * @param saveWifiCredentials Callback to save wifi credentials
   */
  void setSaveWifiCrendentialsCallback(ConfigTypes::saveWifiCredentialsCallback_t saveWifiCredentials)
  {
    saveWifiCredentialsCallback = saveWifiCredentials;
  };

private:
  ProductDebug(const ProductDebug&);
  ProductDebug& operator=(const ProductDebug&);

  ConfigTypes::saveWifiCredentialsCallback_t saveWifiCredentialsCallback{nullptr};
};
} // namespace ProductDebug

#endif /* PRODUCTDEBUG_H_ */
