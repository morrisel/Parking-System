#!/bin/bash

PARAM_NAME=$1
COMPILE="/* gcc ${PARAM_NAME} -o out_${PARAM_NAME:0:-2} */"

awk 'NR==1 {print "'"${COMPILE}"'";} {print}' "${PARAM_NAME}" > temp && mv temp "${PARAM_NAME}"

echo
echo -e "Changes written to the head of the file: ${PARAM_NAME}"
echo -e "Compile command:"
echo -e "${COMPILE}\n"


#eval "awk 'NR==1 {print \"${COMPILE}\"; print \"${LINKER}\"} {print}' $PARAM_NAME"

