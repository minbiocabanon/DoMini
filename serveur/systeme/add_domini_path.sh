#!/bin/sh
# Ajout du repertoire à la variable PAHT
echo 'export PATH=$PATH:~/src/test_git_domini/serveur/systeme/' >> /home/julien/.bashrc 

#On affiche le resultat
echo 'Path :'
echo $PATH
