// part of TigSortGUI
// author: Ulrike Hager

#ifndef TIGWAVEFORMHISTO_H
#define TIGWAVEFORMHISTO_H

#include <TigEvent.h>

#include "TigHistoObject.h"

class TigWaveformHisto : public TigHistoObject
{
 public:
  TigWaveformHisto();
  ~TigWaveformHisto(){};
  bool Evaluate(std::vector<short> pWf);
  bool Initialize();
  virtual  bool ParseInput(std::string line);
  bool ProcessSignal(TigEvent* pEvent);

 protected:
  int mChannel;
  int mAddress;
  int mXbins;

};


#endif /* TIGWAVEFORMHISTO_H */
