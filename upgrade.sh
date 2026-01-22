#!/bin/sh

ls *.yaml | grep -v -F -f exclude | while read YAML; do
  esphome compile $YAML && (esphome upload $YAML &)
done

