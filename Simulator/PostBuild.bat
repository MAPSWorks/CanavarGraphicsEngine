rmdir /s /q "%~1\Resources"
xcopy /E /Y "Simulator\Resources" "%~1\Resources\"
xcopy /E /Y "Engine\Resources" "%~1\Resources\"
xcopy /E /Y "Engine\Dependencies\assimp\bin\assimp-vc142-mt.dll" "%~1"