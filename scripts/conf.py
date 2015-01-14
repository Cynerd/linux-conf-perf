#!/bin/python3
# This file is only loading ../conf.py
import os
import importlib.machinery

confpy = os.path.dirname(__file__) + "/../conf.py"

conf = importlib.machinery.SourceFileLoader("module.name", confpy).load_module()
