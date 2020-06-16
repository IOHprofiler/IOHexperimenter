template <class manufacturedObj> genericGenerator<manufacturedObj>::genericGenerator() {
}

template <class manufacturedObj> genericGenerator<manufacturedObj> &genericGenerator<manufacturedObj>::instance() {
    /// Note that this is not thread-safe!
    static genericGenerator theInstance;
    return theInstance;
}

template <class manufacturedObj> void genericGenerator<manufacturedObj>::regCreateFn(std::string clName, std::shared_ptr<manufacturedObj> (*func)()) {
  registry[clName]=func;
}

template <class manufacturedObj> std::shared_ptr<manufacturedObj> genericGenerator<manufacturedObj>::create(std::string className) const {
  std::shared_ptr<manufacturedObj> ret(nullptr);

  typename FN_registry::const_iterator regEntry = registry.find(className);
  if (regEntry != registry.end()) {
    return (*regEntry).second();
  }
  return ret;
}
