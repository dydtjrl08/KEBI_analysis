void check_pad()
{
  auto tpc = new LAPTpc();
  tpc -> AddPar("prototype.par");
  tpc -> Init();

  auto padplane = (LAPPadPlane *) tpc -> GetPadPlane();
  padplane -> Print();
  padplane -> Clear();

  auto hist_padplane = padplane -> GetHist("0");
  auto numPads = padplane -> GetNumPads();
  //auto hist_padplane = padplane -> GetHist();
  /*
  for (auto iPad=0; iPad<numPads; ++iPad) {
    auto pad = padplane -> GetPad(iPad);
    auto bin = hist_padplane -> FindBin(pad -> GetI(), pad -> GetJ());
    hist_padplane -> SetBinContent(bin,iPad);
  }
  
  */
  auto cvs = padplane -> GetCanvas();
  //hist_padplane -> SetName("PadPlane");
  //hist_padplane -> SetTitle("PadPlane");

  gStyle -> SetPalette(kBird);
  hist_padplane -> Draw("colz");
  //hist_padplane -> Draw("text");
  padplane -> DrawFrame();
  
  padplane -> PadPositionChecker();

}
