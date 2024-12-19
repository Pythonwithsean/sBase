package ui

import (
	"bufio"
	"fmt"
	"os"
)

func Index() {
	var _ string
	reader := bufio.NewReader(os.Stdin)
	fmt.Print("sBase> ")
	_, _ = reader.ReadString('\n')

}
