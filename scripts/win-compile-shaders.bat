@echo off
python win_compile_shaders.py
IF %ERRORLEVEL%==0 (
    cd ..
    cmd /k
) ELSE (
    PAUSE
)