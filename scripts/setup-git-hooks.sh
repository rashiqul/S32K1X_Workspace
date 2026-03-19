#!/bin/bash
# Setup git hooks for automated environment checks

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
HOOKS_DIR="$PROJECT_ROOT/.git/hooks"

echo "🔧 Setting up git hooks..."

# Create post-checkout hook for environment validation
cat > "$HOOKS_DIR/post-checkout" << 'EOF'
#!/bin/bash
# Post-checkout hook - validates development environment

# Only run after actual checkout (not during rebase, etc.)
if [ "$3" = "1" ]; then
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
    
    if [ -f "$PROJECT_ROOT/scripts/setup-dev-environment.sh" ]; then
        echo ""
        echo "🔍 Validating development environment..."
        "$PROJECT_ROOT/scripts/setup-dev-environment.sh"
    fi
fi
EOF

chmod +x "$HOOKS_DIR/post-checkout"

echo "✅ Git hooks installed successfully!"
echo "   - post-checkout: Validates environment after checkout"
