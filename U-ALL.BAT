@echo off
for %%z in (*.prj) do prj2mak %%z 
for %%z in (*.prj) do tc /m %%z
